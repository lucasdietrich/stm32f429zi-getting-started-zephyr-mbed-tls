#include <stdint.h>
#include <errno.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_ip.h>
#include <net/net_if.h>

#include "http_server.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(http_server, LOG_LEVEL_DBG);

K_THREAD_STACK_DEFINE(stack_http_server, HTTP_SERVER_THREAD_STACK_SIZE);

static const char content[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"Connection: close\r\n\r\n"
"<html><body><h3>Hello from stm32f429zi !</h3>"
"</body></html>\n";

/*___________________________________________________________________________*/

c_http_server *c_http_server::p_instance;

struct k_thread c_http_server::http_server_thread;

uint32_t c_http_server::counter = 0u;

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

    LOG_INF("HTTP server up (%d), waiting for connections on port %d ...", serv, HTTP_SERVER_BIND_PORT);

    while (1)
    {
        // todo send connection to subprocess

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        char addr_str[32];
        int req_state = 0;
        const char *data;
        size_t len;

        int client = accept(serv, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client < 0)
        {
            LOG_ERR("Error in accept: %d - continuing", errno);
            continue;
        }

        // convert to text form
        // https://man7.org/linux/man-pages/man3/inet_ntop.3.html
        inet_ntop(client_addr.sin_family, &client_addr.sin_addr, addr_str, sizeof(addr_str));
        
        LOG_INF("Connection #%d from %s", counter++, log_strdup(addr_str));

        // discard
        while (1)
        {
            ssize_t r;
            char c;

            r = recv(client, &c, 1, 0);
            if (r == 0)
            {
                goto close_client;
            }

            if (r < 0)
            {
                if (errno == EAGAIN || errno == EINTR)
                {
                    continue;
                }

                LOG_ERR("Got error %d when receiving from socket", errno);
                goto close_client;
            }

            // looking for end
            if (req_state == 0 && c == '\r')
            {
                req_state++;
            }
            else if (req_state == 1 && c == '\n')
            {
                req_state++;
            }
            else if (req_state == 2 && c == '\r')
            {
                req_state++;
            }
            else if (req_state == 3 && c == '\n')
            {
                LOG_INF("Recv complete");

                break;
            }
            else
            {
                req_state = 0;
            }
        }

        // send response
        data = content;
        len = sizeof(content);
        while (len)
        {
            int sent_len = send(client, data, len, 0);

            if (sent_len == -1)
            {
                LOG_ERR("Error sending data to peer, errno: %d", errno);
                break;
            }
            data += sent_len;
            len -= sent_len;
        }

        LOG_INF("Send complete");

close_client:
        ret = close(client);
        if (ret == 0)
        {
            LOG_INF("Connection from %s closed", log_strdup(addr_str));
        }
        else
        {
            LOG_ERR("Got error %d while closing the socket", errno);
        }
    }
}