#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <sys/types.h>
#include "http_request.h"

#define HTTP_SERVER_BIND_PORT               8080

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

public:
    static uint32_t counter;
    
    c_http_server() {
        p_instance = this;
    }

    static c_http_server* get_instance();

    static void thread_start();

    static void thread(void *, void *, void *);

/*___________________________________________________________________________*/

    static char recv_buffer[HTTP_SERVER_RECV_BUFFER_SIZE];
    static char *send_buffer;

    static ssize_t p_recv;
    static ssize_t p_send;

    static c_http_request request;

    static inline int read_request(int client);

    static inline int parse_request(c_http_request *request);
};

#endif