/**
 * @file http_server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 * 
 * Do following command to send a HTTP request to the server
 *  curl -v http://192.168.10.233:8080/url
 */


#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <sys/types.h>

// see "socket.h" line 679
/* ... code must include fcntl.h before socket.h: */
#include <fcntl.h>

#include <net/socket.h>
#include <net/net_core.h>
#include <net/net_ip.h>
#include <net/net_if.h>

#include <net/http_parser.h>

#include "http_request.h"

/*___________________________________________________________________________*/

#define HTTP_SERVER_BIND_PORT               8080

#define HTTP_SERVER_CONNECTIONS_COUNT       5

#define HTTP_SERVER_TLS                     0

#if HTTP_SERVER_TLS
#define HTTP_SERVER_SOCKET_LISTEN           2
#else
#define HTTP_SERVER_SOCKET_LISTEN           1
#endif

#define HTTP_SERVER_POLL_FDS_ARRAY_SIZE     HTTP_SERVER_SOCKET_LISTEN + HTTP_SERVER_CONNECTIONS_COUNT

#define HTTP_SERVER_THREAD_STACK_SIZE       2048
#define HTTP_SERVER_THREAD_PRIORITY         K_PRIO_PREEMPT(8)

#define HTTP_SERVER_RECV_BUFFER_SIZE        500


/*___________________________________________________________________________*/

class c_http_server
{
private:
    
    static c_http_server *p_instance;

    static struct k_thread http_server_thread;

/*___________________________________________________________________________*/

// State machine

    enum state {
        WAITING,
        RECV,
        SEND,
        CLOSED,
        ERROR,
        PROCESS
    };

    struct http_connection
    {
        // socket file descriptor (client socket)
        int fd;

        // client address and len
        struct sockaddr_in addr;
        socklen_t addr_len;

        // buffers
        char recv_buf[200];
        char send_buf[2000];

        // buffer sizes
        size_t recv_buf_len;
        size_t send_buf_len;

        // current position in the buffer
        char *p_recv;
        char *p_send;

        // parser
        http_parser parser;
    };

    static struct http_connection connections[HTTP_SERVER_CONNECTIONS_COUNT];

    struct http_connection * create_context(int index, int sock)
    {
        if (sock >= 0)
        {
            connections[index].fd = sock;
            
            return &connections[index];
        }

        return nullptr;
    }

/*___________________________________________________________________________*/

public:
    int port;
    
    
    c_http_server() {
        p_instance = this;
    }

    static c_http_server* get_instance();

    static void thread_start();

    static void thread(void *, void *, void *);

/*___________________________________________________________________________*/

 // SOCKET

    // SERVER
    int serv_fd = 0;
    int sec_serv_fd = 0;
    struct sockaddr_in bind_addr;

    // POLL
    struct pollfd fds[HTTP_SERVER_POLL_FDS_ARRAY_SIZE]; 
    int nfds;   // initialize to 0

    // inline function can be difficult to debug (breakpoints ?)
    inline int setup_server(void);

    inline int poll_fds(void);

    inline int accept_incoming_connections(void);

    inline bool is_listening_socket(int fd) const;

    inline bool new_connections_available(void) const;
    
    int set_socket_nonblocking(int fd);

    void compress_fds(void);

    void clear_server(void);

/*___________________________________________________________________________*/



/*___________________________________________________________________________*/

    static char recv_buffer[HTTP_SERVER_RECV_BUFFER_SIZE];
    static char *send_buffer;

    static ssize_t p_recv;
    static ssize_t p_send;

    static c_http_request request;

    static inline size_t parse_request(const char *buffer, size_t len, c_http_request *request);


};

#endif