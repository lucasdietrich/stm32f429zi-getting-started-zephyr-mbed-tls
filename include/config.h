#include <sys_clock.h>

/*___________________________________________________________________________*/

// main
#define MAIN_LOOP_PERIOD                5*60*MSEC_PER_SEC
//#define MAIN_LOOP_PERIOD                0

/*___________________________________________________________________________*/


// app
#define APP_NAME                        "AdecyApp"
#define APP_MAGIC_NUMBER                0xADEC1

/*___________________________________________________________________________*/

#define HW_INIT_LED_BLINK               10
#define HW_INIT_LED_ONOFF               2
#define HW_INIT_LED_MODE                HW_INIT_LED_ONOFF              

#if HW_INIT_LED_MODE == HW_INIT_LED_BLINK
#define HW_INIT_LED_DELAY               100
#else
#define HW_INIT_LED_DELAY               250
#endif

/*___________________________________________________________________________*/

#define TZ_SHIFT                        2*3600
#define APP_NAME_MAX_LENGTH             20        

/*___________________________________________________________________________*/

// sntp
#define SNTP_RETRIES                    3
#define SNTP_RETRY_INTERVAL_MS          250