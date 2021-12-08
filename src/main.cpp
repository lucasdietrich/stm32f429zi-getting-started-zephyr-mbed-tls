#include <zephyr.h>

#include <linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <sys/timeutil.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main_log, LOG_LEVEL_INF); // LOG_LEVEL_DBG

#include "config.h"
#include "app.h"

/*___________________________________________________________________________*/

void main(void)
{
    c_application app = c_application();

    // debug
    LOG_DBG("&app == &c_application::get_instance() : %d", (uint8_t)(&app == &c_application::get_instance()));

    LOG_DBG("app name : %s", app.config.name);
    LOG_DBG("app magic number : %x", app.config.magic_number);

    // init app
    app.init();

    LOG_INF("Application up & running");

    struct tm time_infos;

    while(1)
    {
#if MAIN_LOOP_PERIOD
        app.get_time_infos(&time_infos);

        printk("Local (Europe/Paris) Date and time : %04d/%02d/%02d %02d:%02d:%02d\n",
               time_infos.tm_year + 1900, time_infos.tm_mon + 1, time_infos.tm_mday,
               time_infos.tm_hour, time_infos.tm_min, time_infos.tm_sec);
        printk("iface up = %d", net_if_is_up(net_if_get_default()) ? 1 : 0);

        k_sleep(K_MSEC(MAIN_LOOP_PERIOD));
#else
        k_yield();
#endif
    }
}

/*
    k_sleep(K_MSEC(15000));

    down_if();
*/