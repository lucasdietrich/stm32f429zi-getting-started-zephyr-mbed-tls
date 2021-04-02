#ifndef _APP_H
#define _APP_H

#include <stdint.h>
#include <sys/timeutil.h>

#include "config.h"
#include "types.h"
#include "discovery.h"

/*___________________________________________________________________________*/

class c_discovery;

/*___________________________________________________________________________*/

#define TZ_SHIFT        DEFAULT_TIMEZONE_SHIFT

#define SNTP_RETRIES    DEFAULT_SNTP_RETRIES
#define SNTP_RETRY_INTERVAL_MS  DEFAULT_SNTP_RETRY_INTERVAL_MS

/*___________________________________________________________________________*/

static const char adecy_app_name[DEFAULT_APP_NAME_MAX_LENGTH] = "AdecyApp";

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

    //c_application(const c_application&) = delete;               // prevent copy

    //c_application& operator=(const c_application&) = delete;    // prevent assignment ?

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