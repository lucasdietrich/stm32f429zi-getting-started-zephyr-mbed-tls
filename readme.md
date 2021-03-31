# Nucleo f429zi

Current system model

![./system_model.png](./pics/system_model.png)

# Todo

## file system fs_read / fs_write

## set time

## k_work_init to sync tasks

- Workqueues : https://docs.zephyrproject.org/1.9.0/kernel/threads/workqueues.html

## Threading Zephyr

Zephyr Scheduling : Interesting and important : https://docs.zephyrproject.org/1.12.0/kernel/threads/scheduling.html
- Threading API : https://docs.zephyrproject.org/latest/reference/kernel/threads/index.html
- Scheduling API : https://docs.zephyrproject.org/latest/reference/kernel/scheduling/index.html

Polling API : https://docs.zephyrproject.org/1.9.0/kernel/other/polling.html

About `main` thread : https://docs.zephyrproject.org/1.12.0/kernel/threads/system_threads.html

# Info

## Link to : https://os.mbed.com/platforms/ST-Nucleo-F429ZI/

## DTS file - for hw.cpp
 
*.dts* file : `nucleo_f429zi.dts` located here : `.platformio\packages\framework-zephyr\boards\arm\nucleo_f429zi`

- *LED0*, *LED1*, *LED2* : leds *green*, *blue*, *red*
- *SW0* : user button

## .c -> .cpp

In order to use *.cpp* files rather than *.c* files, completely delete the *.pio* (build) directory.

## SNTP

Zephyr https://docs.zephyrproject.org/latest/reference/networking/sntp.html#group__sntp_1ga945936e5164bbd959cfa666ceacdac13

## Meaning of NET_EVENT_ETHERNET_CARRIER_ON & NET_EVENT_ETHERNET_CARRIER_OFF ?

## Zephyr Kernel API

https://docs.zephyrproject.org/1.9.0/api/kernel_api.html#_CPPv319k_delayed_work_initP14k_delayed_work16k_work_handler_t

## Zephyr minimal configuration

- for IoT
- 32 bits ?
- min 8kb on ram

Comparison Free RTOS & Zephyr RTOS & mbed os : https://www.reddit.com/r/embedded/comments/in7rjy/zephyr_vs_freertos_for_embedded_iot_project/

---

# Issues :

Find issues in the code by typing : `ISSUE 001` for example.

## 001. undefined reference to *net_config_init_app*
When disabling : `CONFIG_NET_CONFIG_AUTO_INIT=n`

And when we call manually `net_config_init_app(NULL, "app");` or `net_config_init("app", NET_CONFIG_NEED_IPV4, 1000);` the linker says `undefined reference to net_config_init`.

Even when including `#include <net/net_config.h>` and all the other headers : 

```
#include <net/net_if.h>
#include <net/net_core.h>
#include <net/net_context.h>
#include <net/net_mgmt.h>
```

Maybe cmake, missing binaries, configuration ?

Waiting, I'm setting `CONFIG_NET_CONFIG_AUTO_INIT=y` and don't calling these functions.

### 002. <err> os: CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE is 0

Need