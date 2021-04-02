#include <sys_clock.h>

/*___________________________________________________________________________*/

// main
#define MAIN_LOOP_PERIOD                15*MSEC_PER_SEC

/*___________________________________________________________________________*/

// app
#define APP_NAME                        "AdecyApp"
#define APP_MAGIC_NUMBER                0xADEC1

/*___________________________________________________________________________*/

#define TZ_SHIFT                        2*3600
#define APP_NAME_MAX_LENGTH             20        

/*___________________________________________________________________________*/

// sntp
#define SNTP_RETRIES                    3
#define SNTP_RETRY_INTERVAL_MS          250