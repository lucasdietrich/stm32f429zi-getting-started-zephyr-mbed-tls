#include <zephyr.h>

#include <linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <sys/timeutil.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main_log, LOG_LEVEL_DBG);

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

    LOG_INF("System fully initialized");

    struct tm time_infos;

    while(1) 
    {
        app.get_time_infos(&time_infos);

        printf("Local (Europe/Paris) Date and time : %04d/%02d/%02d %02d:%02d:%02d\n",
               time_infos.tm_year + 1900, time_infos.tm_mon + 1, time_infos.tm_mday,
               time_infos.tm_hour, time_infos.tm_min, time_infos.tm_sec);

        k_sleep(K_MSEC(MAIN_LOOP_PERIOD));
    }
}

/*
    k_sleep(K_MSEC(15000));

    down_if();
*/