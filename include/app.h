#ifndef _APP_H
#define _APP_H

#include <stdint.h>

#include <sys/timeutil.h>

#define TZ_SHIFT    2*60*60


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
public:
    static c_application * p_instance;

    void set_time(uint64_t abs_time, uint32_t sntp_instant);
    
public:
    struct app_config_t config;
    struct app_time_t time;
    struct tm time_infos;

    c_application();

    /*
    static c_application* get_instance(void);
    */

    uint64_t get_time(void);

    struct tm * get_time_infos(void);

    void init(void);
};


#endif