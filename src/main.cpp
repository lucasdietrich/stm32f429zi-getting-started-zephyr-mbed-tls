#include <zephyr.h>

#include <linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main_log, LOG_LEVEL_DBG);

#include "app.h"

c_application app;

void main(void)
{    
    // init app
    //c_application::get_instance()->init();

    app.init();

    while(1);
}

/*
    k_sleep(K_MSEC(15000));

    down_if();
*/