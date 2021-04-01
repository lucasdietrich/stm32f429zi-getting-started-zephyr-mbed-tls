#ifndef _APP_H
#define _APP_H

#include <stdint.h>
#include <sys/timeutil.h>

#include "config.h"
#include "discovery.h"

#define TZ_SHIFT    DEFAULT_TIMEZONE_SHIFT


struct app_config_t {
    const char *name;           // application name
};

struct app_time_t {
    uint64_t abs_time;          // absolute time in seconds retrieved by SNTP
    uint32_t sntp_instant;      // local instant of the sntp request
};

// make this class a *singleton*
class c_application
{
protected:
    static c_application * p_instance;

    c_discovery discovery;

    void set_time(uint64_t abs_time, uint32_t sntp_instant);
    
    struct app_config_t config;
    struct app_time_t time;
    struct tm time_infos;


public:
    c_application();

    /*
    static c_application* get_instance(void);
    */

    uint64_t get_time(void);

    struct tm * get_time_infos(void);

    void init(void);
};


#endif