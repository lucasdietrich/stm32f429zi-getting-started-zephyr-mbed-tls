#include <zephyr.h>

#include <linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(main_log, LOG_LEVEL_DBG);

#include "hw.h"
#include "if.h"

void main(void)
{
    LOG_INF("main Init ...");

    hw_init_leds();

    k_sleep(K_MSEC(1000));

    init_if();

    k_sleep(K_MSEC(15000));

    down_if();

    while(1) {
        
    }
}