#ifndef _DISCOVERY_H
#define _DISCOVERY_H

#include "config.h"

#define DISCOVERY_PORT DEFAULT_DISCOVERY_PORT

#define DISCOVERY_THREAD_PRIORITY       8
#define DISCOVERY_THREAD_STACK_SIZE     750

class c_discovery
{
private :
    uint16_t port;

    uint16_t counter;
    
    static c_discovery * p_instance;
    
    static struct k_thread discovery_thread;

public:
    c_discovery() {
        p_instance = this;
    }


    void set_port(uint16_t port);

    void show_port(void);

    static c_discovery *get_instance(void);

    static void thread_start(void);

    static void thread(void *, void *, void *);
};

#endif