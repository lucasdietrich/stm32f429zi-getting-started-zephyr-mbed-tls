#ifndef _APP_H
#define _APP_H

#include <stdint.h>
#include <sys/timeutil.h>

#include "config.h"
#include "discovery.h"

/*___________________________________________________________________________*/

#define TZ_SHIFT        DEFAULT_TIMEZONE_SHIFT

#define SNTP_RETRIES    DEFAULT_SNTP_RETRIES
#define SNTP_RETRY_INTERVAL_MS  DEFAULT_SNTP_RETRY_INTERVAL_MS

/*___________________________________________________________________________*/

struct app_config_t {
    const char *name;                   // application name
    uint32_t magic_number;
};

struct app_time_t {
    uint64_t abs_time;          // absolute time in seconds retrieved by SNTP
    uint32_t sntp_instant;      // local instant of the sntp request
};

static const char adecy_app_name[20] = "AdecyApp";

/*___________________________________________________________________________*/

/**
 * @brief This class initialize the hardware and the the system main threads
 * 
 * This class is not yet a singleton, but should be changed in this way, check :
 *  - https://lonkamikaze.github.io/2019/10/05/embedded-cxx-singletons
 *  - https://www.embeddedrelated.com/showarticle/691.php
 * 
 */
class c_application
{
private:
    /* SINGLETON FEATURES */
    static c_application * p_instance;
    
public:

    // set this constructor private
    // read about this : https://lonkamikaze.github.io/2019/10/05/embedded-cxx-singletons
    c_application();    

    /* SINGLETON FEATURES */
    // two function 
    static c_application& get_instance()
    {
        return *p_instance;
    }

    // c_application(const c_application&) = delete;               // prevent copy

    // c_application& operator=(const c_application&) = delete;    // prevent assignment ?

/*___________________________________________________________________________*/
    
    struct app_config_t config;
    struct app_time_t time;

    // modules
    c_discovery discovery;

/*___________________________________________________________________________*/

    // init
    void init();

/*___________________________________________________________________________*/

    // TIME
    void set_time(uint64_t abs_time, uint32_t sntp_instant);
    uint64_t get_time() const;
    void get_time_infos(struct tm *p_time_infos) const;
};


#endif