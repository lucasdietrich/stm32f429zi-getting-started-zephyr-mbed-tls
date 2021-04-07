#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#define HTTP_SERVER_BIND_PORT       8080

#define HTTP_SERVER_THREAD_STACK_SIZE       8192
#define HTTP_SERVER_THREAD_PRIORITY         K_PRIO_PREEMPT(8)


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
};

#endif