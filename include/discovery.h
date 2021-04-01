#ifndef _DISCOVERY_H
#define _DISCOVERY_H

#include "config.h"

#define DISCOVERY_PORT DEFAULT_DISCOVERY_PORT

#define DISCOVERY_THREAD_PRIORITY       8
#define DISCOVERY_THREAD_STACK_SIZE     750

class c_discovery
{
protected :
    uint16_t port;

    uint16_t counter;
    
    static c_discovery * instance;
    
    static struct k_thread discovery_thread;

public:
    c_discovery(void) {
        instance = this;
    }

    void set_port(uint16_t port);

    static c_discovery * get_instance(void);

    static void thread_start(void);

    static void thread(void *, void *, void *);
};

#endif