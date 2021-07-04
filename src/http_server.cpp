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

c_http_request c_http_server::requests[HTTP_SERVER_CONNECTIONS_COUNT];
c_http_response c_http_server::responses[HTTP_SERVER_CONNECTIONS_COUNT];

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
    int received;
    bool compress_fds_requested = false;
    const int current_size = nfds;

    LOG_DBG("Polling on %d/%u sockets ...", nfds, HTTP_SERVER_POLL_FDS_ARRAY_SIZE); // HTTP_SERVER_POLL_FDS_ARRAY_SIZE = ARRAY_SIZE(fds)

    const int timeout = 5*60*1000; // abandon after 3 minutes

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

    // poll on each fd
    for(int_fast16_t i = 0; i < current_size; i++)
    {
        if (fds[i].revents == 0)    // no event ignore
        {
            continue;
        }

        if (0 == (fds[i].revents & (POLLIN | POLLOUT)))  // fds[i].revents & (POLLIN | POLLOUT)
        {
            LOG_ERR("Invalid revents value %d for socket (%d)", fds[i].revents, fds[i].fd);

            return -1;
        }

        // todo
        if (fds[i].revents & POLLOUT)
        {
            // ready for writability
            LOG_DBG("ready for writability");
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

            // prepare
           
           c_http_request * request = &requests[i - 1];

            do
            {
                LOG_DBG("Event on client socket (%d)", fd);

                // set max recv size to 50 to test appending
                // maybe MIN value must be CONFIG_NET_BUF_DATA_SIZE (as for CONFIG_NET_BUF_DATA_SIZE=128), in order to free a buffer at each read ?
                uint16_t recv_size = MIN(request->remaining, CONFIG_NET_BUF_DATA_SIZE); // CONFIG_NET_BUF_DATA_SIZE

                received = recv(fd, request ->p, recv_size, 0);
                if (received < 0)    // error
                {
                    // errno = -received
                    if (errno == EWOULDBLOCK)
                    {
                        LOG_DBG("recv would block, break");
                    }
                    else
                    {
                        LOG_DBG("recv failed with errno = %d", received);

                        close_connection_requested = true;
                    }
                    
                    break;
                }

                if (received == 0) // close connection requested
                {
                    LOG_DBG("connection closed");

                    close_connection_requested = true;

                    break;
                }
                else if (received > 0) // bytes received
                {
                    LOG_DBG("recv %d bytes on socket %d", received, fd);

                    request->parse_appended(received);

                    if (request->is_complete())
                    {
                        LOG_INF("request complete");

                        handle_request(request);
                    }
                    // if (http_body_is_final(&request->parser))
                    // {
                    //     int sent = send(fds[i].fd, content, strlen(content), 0);

                    //     LOG_DBG("async sent returned with %d", sent);
                    // }

#if HTTP_SERVER_SINGLE_READ
                    break;
#endif
                }

                // recv len not fully received
                /*
                if (received < (int) sizeof(buf))
                {
                    break; 
                }
                */

            } while(true);

            // close if requested
            if (close_connection_requested)
            {
                close(fds[i].fd);

                fds[i].fd = -1;

                compress_fds_requested = true;
            }
            else
            {
                // if request complete
                LOG_INF("%d", request->parser.state);
                if (request->is_complete())
                {
                    LOG_DBG("Request complete");
                }
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

    // TODO maybe simplify with pre existing accept function
    if (nfds == HTTP_SERVER_POLL_FDS_ARRAY_SIZE) // too many connections, need to refuse this unwished connection
    {
        // https://stackoverflow.com/questions/16590847/how-can-i-refuse-a-socket-connection-in-c/54542397

        int refused = accept(serv_fd, nullptr, nullptr);

        if (refused >= 0)
        {
            LOG_WRN("Too many connections, refusing recently received ...");

            close(refused);
        }
        else // debug
        {
            // do(6) / 5
            LOG_ERR("Accept failed on unwished connection");
        }

        return 0;
    }

    do {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(struct sockaddr_in);

        new_client_fd = accept(serv_fd, (struct sockaddr*) &addr, &addr_len);

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

        char addr_str[NET_IPV4_ADDR_LEN];
        inet_ntop(addr.sin_family, &addr.sin_addr, addr_str, sizeof(addr_str));

        LOG_DBG("Accept new connection (%d) from %s", new_client_fd, log_strdup(addr_str));

        // https://man7.org/linux/man-pages/man2/accept.2.html#CONFORMING_TO
        ret = set_socket_nonblocking(new_client_fd);
        if (ret < 0)
        {
            close(new_client_fd);
            return ret;
        }

        // adding new incoming connection to fds
        fds[nfds].fd = new_client_fd;
        fds[nfds].events = POLLIN;   // | POLLOUT : poll for readability and writeability

        // clear request
        requests[nfds - 1].clear();

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

int c_http_server::handle_request(c_http_request *request)
{
    return 0;
}

void c_http_server::dispath_request(c_http_request &request)
{

}

/*___________________________________________________________________________*/