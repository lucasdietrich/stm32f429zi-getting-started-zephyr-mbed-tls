#include <stdint.h>
#include <errno.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_ip.h>
#include <net/net_if.h>
#include <net/http_parser.h>

#include "http_server.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(http_server, LOG_LEVEL_INF);

K_THREAD_STACK_DEFINE(stack_http_server, HTTP_SERVER_THREAD_STACK_SIZE);

static const char content[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n\r\n"
"<html><body><h3>Hello from stm32f429zi !</h3>"
"</body></html>";

/*___________________________________________________________________________*/

c_http_server *c_http_server::p_instance;

struct k_thread c_http_server::http_server_thread;

uint32_t c_http_server::counter = 0u;

char c_http_server::recv_buffer[HTTP_SERVER_RECV_BUFFER_SIZE];
char *c_http_server::send_buffer;

ssize_t c_http_server::p_recv = 0u;
ssize_t c_http_server::p_send = 0u;

c_http_request c_http_server::request = c_http_request();

/*___________________________________________________________________________*/

c_http_server *c_http_server::get_instance()
{
    return p_instance;
}

void c_http_server::thread_start(void)
{
    k_tid_t thread_id = k_thread_create(&http_server_thread, stack_http_server,
                                        K_THREAD_STACK_SIZEOF(stack_http_server),
                                        c_http_server::thread,
                                        NULL, NULL, NULL,
                                        HTTP_SERVER_THREAD_PRIORITY, 0, K_FOREVER);

#if CONFIG_THREAD_NAME

    k_thread_name_set(thread_id, "http_server");

#endif

    k_thread_start(thread_id);
}

void c_http_server::thread(void *, void *, void *)
{
    int serv;
    int ret;

    struct sockaddr_in bind_addr;

    serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serv < 0)
    {
        LOG_ERR("Failed to create TCP socket (%d): %d", serv, errno);

        return;
    }

    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(HTTP_SERVER_BIND_PORT); 
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY); // https://stackoverflow.com/a/16510000/15309977

    ret = bind(serv, (const sockaddr *) &bind_addr, sizeof(sockaddr_in));
    if (ret < 0)
    {
        LOG_ERR("Failed to bind TCP socket (%d): %d", serv, errno);
        return;
    }

    ret = listen(serv, 3);
    if (ret < 0)
    {
        LOG_ERR("Failed to listen TCP socket (%d): %d", serv, errno);
        return;
    }    

    LOG_INF("HTTP server up (%d), waiting for TCP connections on port %d ...", serv, HTTP_SERVER_BIND_PORT);

    while (1)
    {
        // todo send connection to subprocess

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char addr_str[32];
        size_t len;

        int client = accept(serv, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client < 0)
        {
            LOG_ERR("Error in accept: %d - continuing", errno);
            continue;
        }

/*___________________________________________________________________________*/

        // convert to text form
        // https://man7.org/linux/man-pages/man3/inet_ntop.3.html
        inet_ntop(client_addr.sin_family, &client_addr.sin_addr, addr_str, sizeof(addr_str));
        
        LOG_INF("Connection #%d from %s", counter++, log_strdup(addr_str));

        p_recv = 0u;
        p_send = 0u;

        // read request
        ret = read_request(client);
        if (ret != 0)
        {
            LOG_ERR("failed to read request");

            goto close_client;
        }

        // parse request
        ret = parse_request(recv_buffer, p_recv, &request);
        if (ret <= 0)
        {
            LOG_ERR("failed to parse request");

            goto close_client;
        }

        // send response
        send_buffer = (char*) content;
        len = strlen(content);
        p_send = 0u;

        while (len)
        {
            int sent_len;

            sent_len = send(client, &send_buffer[p_send], len, 0);

            if (sent_len < 0)
            {
                LOG_ERR("Error sending data to peer, errno: %d", errno);
                break;
            }

            p_send += sent_len;
            len -= sent_len;
        }

        LOG_DBG("Send complete");

close_client:
        ret = close(client);
        if (ret == 0)
        {
            LOG_DBG("Connection from %s closed", log_strdup(addr_str));
        }
        else
        {
            LOG_ERR("Got error %d while closing the socket", errno);
        }
    }
}

inline int c_http_server::read_request(int client)
{
    int max_len = sizeof(recv_buffer) - 1; // keep space for termination string

    recv_buffer[sizeof(recv_buffer) - 1] = 0;

    while (1)
    {
        ssize_t recv_len;

        recv_len = recv(client, &recv_buffer[p_recv], max_len - p_recv, 0);

        if (recv_len < 0)
        {
            if (errno == EAGAIN || errno == EINTR)
            {
                continue;
            }

            LOG_ERR("Got error %d when receiving from socket", errno);

            return -1;
        }

        p_recv += recv_len;

        // HTTP request termination pattern
        if (0u == strncmp(&recv_buffer[p_recv - 4], "\r\n\r\n", 4u))
        {
            LOG_DBG("Recv complete, termination pattern found, size : %d", p_recv);

            break;
        }
        else if (recv_len == 0)
        {
            LOG_ERR("recv_len = 0 but termination pattern not found");

            return -1;
        }
        else if (max_len == p_recv)
        {
            LOG_ERR("buffer full but termination pattern not found");

            return -1;
        }
    }

    return 0u;
}

static char http_current_url[120];

int http_on_url(struct http_parser *, const char *at, size_t length)
{
    LOG_HEXDUMP_DBG(at, length, "on_url");

    if (length >= ARRAY_SIZE(http_current_url))
    {
        length = ARRAY_SIZE(http_current_url) - 1;
    }

    strncpy(http_current_url, at, length);

    http_current_url[length] = '\0';

    return 0;
}

inline size_t c_http_server::parse_request(const char *buffer, size_t len, c_http_request *request)
{
    LOG_HEXDUMP_DBG(buffer, len, "parse_request");

    http_parser parser;

    http_parser_init(&parser, http_parser_type::HTTP_REQUEST);

    const http_parser_settings settings = {
        .on_url = http_on_url
    };

    size_t parsed = http_parser_execute(&parser, &settings, buffer, len);

    // LOGGING
    const char *method_str = log_strdup(http_method_str((enum http_method) parser.method));
    const char *url_str = log_strdup(http_current_url);
    const char *errno_str = log_strdup(http_errno_name(HTTP_PARSER_ERRNO(&parser)));

    LOG_INF("HTTP request %s %s : parsed %d/%d errno %d:%s", method_str, url_str, parsed, len, parser.http_errno, errno_str);

    return parsed;
}