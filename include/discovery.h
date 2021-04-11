#ifndef _DISCOVERY_H
#define _DISCOVERY_H

#include "config.h"

#include <net/net_if.h>

#include "types.h"

/*___________________________________________________________________________*/

#define DISCOVERY_PORT 5001

#define DISCOVERY_LOOPBACK  0

#define DISCOVERY_THREAD_PRIORITY       K_PRIO_PREEMPT(8)
#define DISCOVERY_THREAD_STACK_SIZE     750

/*___________________________________________________________________________*/

class c_discovery
{
private :
    uint16_t port;
    
    static c_discovery * p_instance;
    
    static struct k_thread discovery_thread;

/*___________________________________________________________________________*/

public:
    c_discovery() {
        p_instance = this;
    }


    void set_port(uint16_t port);

    void show_port(void);

    static c_discovery *get_instance(void);

    /*___________________________________________________________________________*/

    static uint32_t counter;

    static char recv_buffer[60];
    static char send_buffer[60];

    static void thread_start(void);

    static void thread(void *, void *, void *);

    /*___________________________________________________________________________*/
    
    static inline size_t build_response(void);
};

#endif