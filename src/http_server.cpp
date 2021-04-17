#include <stdint.h>
#include <errno.h>

#include "http_server.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(http_server, LOG_LEVEL_DBG);

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

/*___________________________________________________________________________*/

char c_http_server::recv_buffer[HTTP_SERVER_RECV_BUFFER_SIZE];
char *c_http_server::send_buffer;

ssize_t c_http_server::p_recv = 0u;
ssize_t c_http_server::p_send = 0u;

c_http_request c_http_server::request = c_http_request();

struct c_http_server::http_connection c_http_server::connections[HTTP_SERVER_CONNECTIONS_COUNT];

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

// implementation : https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select
void c_http_server::thread(void *, void *, void *)
{
    int ret = p_instance->setup_server();

    do {

        ret = p_instance->poll_fds();

    } while(true);

    p_instance->clear_server();
}

/*___________________________________________________________________________*/

inline int c_http_server::setup_server(void)
{
    int ret;

    // clear all data first
    clear_server();

    // socket open
    serv_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_fd < 0)
    {
        LOG_ERR("Failed to create TCP socket (%d): %d", serv_fd, errno);

        return serv_fd;
    }

    // allow socket descriptor to be reuseable
    int on = 1;
    ret = setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (ret < 0)
    {
        LOG_ERR("setsockopt() failed on socket (%d): %d", serv_fd, errno);
        
        return ret;
    }

    // set socked non blocking
    ret = set_socket_nonblocking(serv_fd);
    if (ret < 0)
    {
        return ret;
    }

    // bind
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(HTTP_SERVER_BIND_PORT); 
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY); // https://stackoverflow.com/a/16510000/15309977

    ret = bind(serv_fd, (const sockaddr *) &bind_addr, sizeof(sockaddr_in));
    if (ret < 0)
    {
        LOG_ERR("Failed to bind TCP socket (%d) : %d", serv_fd, errno);

        return ret;
    }

    // listen
    ret = listen(serv_fd, HTTP_SERVER_CONNECTIONS_COUNT);
    if (ret < 0)
    {
        LOG_ERR("Failed to listen TCP socket (%d): %d", serv_fd, errno);

        return ret;
    }    

    LOG_INF("HTTP server up (%d), listening for TCP connections on port %d ...", serv_fd, HTTP_SERVER_BIND_PORT);

    fds[0].fd = serv_fd;
    fds[0].events = POLLIN;

    nfds = 1;

    return 0;
}

inline int c_http_server::poll_fds(void)
{
    int ret;
    bool compress_fds_requested = false;
    const int current_size = nfds;

    LOG_DBG("Polling on %d/%u sockets ...", nfds, HTTP_SERVER_POLL_FDS_ARRAY_SIZE); // HTTP_SERVER_POLL_FDS_ARRAY_SIZE = ARRAY_SIZE(fds)

    const int timeout = 3*60*1000; // abandon after 3 minutes

    ret = poll(fds, nfds, timeout); // todo change
    if (ret == 0)
    {
        LOG_DBG("poll() expired after %d ms", timeout);

        return ret;
    }
    else if (ret < 0)
    {
        LOG_ERR("Failed to poll on fds[%d]", nfds);

        return ret;
    }

    for(int_fast16_t i = 0; i < current_size; i++)
    {
        if (fds[i].revents == 0)    // no event ignore
        {
            continue;
        }

        if (fds[i].revents != POLLIN)
        {
            LOG_ERR("Invalid revents value %d for socket (%d)", fds[i].revents, fds[i].fd);

            return -1;
        }

        const int fd = fds[i].fd;

        // event on server socket (new incoming connections to accept)
        if (is_listening_socket(fd))
        {
            accept_incoming_connections();
        }
        // event on client sockets (incoming data)
        else
        {
            bool close_connection_requested = false;

            do
            {
                LOG_DBG("Receiving on socked (%d)", fd);

                char buf[50];
                ret = recv(fd, buf, sizeof(buf), 0);
                if (ret < 0)
                {
                    // errno = -ret
                    if (errno == EWOULDBLOCK)
                    {
                        LOG_DBG("recv would block");
                    }
                    else
                    {
                        LOG_DBG("recv failed with errno = %d", ret);

                        close_connection_requested = true;
                    }
                    
                    break;
                }

                if (ret == 0) // connection closed
                {
                    LOG_DBG("connection closed");

                    close_connection_requested = true;

                    break;
                }
                else if (ret > 0) // bytes received
                {
                    LOG_DBG("recv %d bytes on socket %d", ret, fd);
                }

                // TODO : break or not if buffer is not full ?
                if (ret < (int) sizeof(buf))
                {
                    break; 
                }

            } while(true);

            if (close_connection_requested)
            {
                close(fds[i].fd);

                fds[i].fd = -1;

                compress_fds_requested = true;
            }
        }
    }

    if (compress_fds_requested)
    {
        compress_fds();

        LOG_DBG("compressing fds");
    }

    return ret;
}

inline int c_http_server::accept_incoming_connections(void)
{
    int new_client_fd;
    int ret;

    do {
        new_client_fd = accept(serv_fd, NULL, NULL);        
        if (new_client_fd < 0)
        {
            // accepting again would block (EWOULDBLOCK == EAGAIN)
            if (errno == EWOULDBLOCK)
            {
                break;
            }
            // any other error is critical
            else
            {
                LOG_ERR("Failed to accept() on socket %d : %d", serv_fd, errno);
                
                return new_client_fd;
            }
        }

        LOG_DBG("Accept new connection (%d)", new_client_fd);

        // https://man7.org/linux/man-pages/man2/accept.2.html#CONFORMING_TO
        ret = set_socket_nonblocking(new_client_fd);
        if (ret < 0)
        {
            close(new_client_fd);
            return ret;
        }

        // adding new incoming connection to fds
        fds[nfds].fd = new_client_fd;
        fds[nfds].events = POLLIN;

        nfds++;

    } while(new_connections_available());

    return 0;
}

inline bool c_http_server::is_listening_socket(int fd) const
{
#if HTTP_SERVER_TLS
    return (fd == serv_fd) || (fd == sec_serv_fd);
#else
    return (fd == serv_fd);
#endif
}

inline bool c_http_server::new_connections_available(void) const
{
    return nfds < HTTP_SERVER_POLL_FDS_ARRAY_SIZE;
}

/**
 * @brief Enable async socket API
 * 
 * - set socket attribute flag O_NONBLOCK
 * - must be applied on accepted client fd while they don't inherit parent socket options
 *      @see : https://man7.org/linux/man-pages/man2/accept.2.html#CONFORMING_TO
 * 
 * @param fd socket file descriptor
 * 
 * @return int 0 if success;
 */
int c_http_server::set_socket_nonblocking(int fd)
{
    int ret;
    // int opt;

    // read socket option
    // READ is not necessary since the only option configurable is blocking flag O_NONBLOCK
    /*
    opt = fcntl(fd, F_GETFL, 0);
    if (opt < 0)
    {
        LOG_ERR("fcntl(fd, F_GETFL, 0) failed on socket (%d): %d", fd, errno);

        return opt;
    }
    */
    
    // add flag O_NONBLOCK
    ret = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (ret < 0)
    {
        LOG_ERR("fcntl(fd, F_SETFL, O_NONBLOCK) failed on socket (%d) : %d", fd, errno);
    }

    return ret;
}

void c_http_server::compress_fds(void)
{
    for (int_fast16_t i = 0; i < nfds; i++)
    {
        if (fds[i].fd == -1)
        {
            for (int_fast16_t j = i; j < nfds - 1; j++)
            {
                fds[j].fd = fds[j + 1].fd;
            }
            i--;
            nfds--;
        }
    }
}

void c_http_server::clear_server(void)
{
    // reset poll structure
    memset((void*) fds, 0 , sizeof(fds));
    nfds = 0;

    /*
    memset((void*) connections, 0, sizeof(connections));
    */
}

/*___________________________________________________________________________*/



/*___________________________________________________________________________*/

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
