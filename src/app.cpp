#include "app.h"

#include "hw.h"
#include "if.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

c_application::c_application()
{
    c_application::p_instance = this;

    c_application::set_time(0, 0);
}

// Measuring Time with Normal Precision
// https://docs.zephyrproject.org/1.9.0/kernel/timing/clocks.html#measuring-time-with-normal-precision
void c_application::set_time(uint64_t abs_time, uint32_t sntp_instant)
{
    if (sntp_instant != 0)
    {
        time.sntp_instant = sntp_instant;
    }
    else
    {
        time.sntp_instant = k_uptime_get() / MSEC_PER_SEC;  // current time in seconds
    }
    time.abs_time = abs_time;
}

uint64_t c_application::get_time(void)
{
    return time.abs_time + k_uptime_get() / MSEC_PER_SEC - time.sntp_instant;
}

struct tm * c_application::get_time_infos(void)
{
    time_t timestamp = this->get_time() + TZ_SHIFT;
    //struct tm *timeInfos = gmtime(&timestamp);      // local time

    // time
    // https://koor.fr/C/ctime/gmtime.wp
    gmtime_r((const time_t *)(const int64_t *) &timestamp, &time_infos);

    return &time_infos;
}

/*
static c_application* c_application::get_instance(void) {
    if (c_application::p_instance != nullptr) {
        return c_application::p_instance;
    }

    // return c_application(nullptr);
    
    return nullptr;
}
*/

struct k_poll_signal signal_net_addr_up;

void c_application::init(void)
{
    // base init
    hw_init_leds();
    init_if();

    k_poll_signal_init(&signal_net_addr_up);

    struct k_poll_event events[1] =
    {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL,
                                 K_POLL_MODE_NOTIFY_ONLY,
                                 &signal_net_addr_up)
    };

    // waiting to net if to have an ip address
    k_poll(events, 1, K_FOREVER);

    k_sleep(K_MSEC(250));

    uint64_t abs_time_secs;

    // 3 retries
    for(uint_fast8_t retries = 3; retries > 0; retries--)
    {
        LOG_INF("SNTP try %d / 3", 1 + 3 - retries);

        abs_time_secs = get_sntp_time();

        if(abs_time_secs != 0)
        {
            this->set_time(abs_time_secs, 0);

            break;
        }

        k_sleep(K_MSEC(250));
    }

    // udp discovery thread start
    discovery.set_port(DISCOVERY_PORT);
    discovery.thread_start();
}