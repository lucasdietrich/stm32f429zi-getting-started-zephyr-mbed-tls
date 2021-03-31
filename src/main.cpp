#include <zephyr.h>

#include <linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <sys/timeutil.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main_log, LOG_LEVEL_DBG);

#include "app.h"

c_application app;

void main(void)
{    
    // init app
    //c_application::get_instance()->init();

    app.init();

    LOG_INF("System fully initialized");

    while(1) 
    {
        struct tm * timeInfos = app.get_time_infos();

        printf("Local (Europe/Paris) Date and time : %04d/%02d/%02d %02d:%02d:%02d\n",
               timeInfos->tm_year + 1900, timeInfos->tm_mon + 1, timeInfos->tm_mday,
               timeInfos->tm_hour, timeInfos->tm_min, timeInfos->tm_sec);

        k_sleep(K_MSEC(1000));

        // k_cpu_idle();
    }
}

/*
    k_sleep(K_MSEC(15000));

    down_if();
*/