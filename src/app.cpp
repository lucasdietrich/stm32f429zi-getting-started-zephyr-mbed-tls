#include "app.h"

#include "hw.h"
#include "if.h"

c_application::c_application() {
    c_application::p_instance = this;

    
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

void c_application::init(void)
{
    hw_init_leds();
    init_if();

    k_sleep(K_MSEC(10000));

    get_sntp_time();   
}