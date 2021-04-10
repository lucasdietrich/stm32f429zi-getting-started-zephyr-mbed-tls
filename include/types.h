#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <net/net_if.h>

/*___________________________________________________________________________*/

// APPLICATION

struct __attribute__((__packed__)) app_config_t {
    const char *name;                   // application name
    uint32_t magic_number;
};

struct __attribute__((__packed__)) app_time_t {
    uint64_t abs_time;          // absolute time in seconds retrieved by SNTP
    uint32_t sntp_instant;      // local instant of the sntp request
};

struct __attribute__((__packed__)) app_stats_t {
    uint64_t ready_time;        // instant where the application is totally up, devide by MSEC_PER_SEC to get value in second
};

/*___________________________________________________________________________*/

// UDP DISCOVERY

// todo use : 
// #pragma packed(1)
// #pragma endregion
struct __attribute__((__packed__)) udp_response_cmplx
{
    uint32_t ip;                        // ip
    struct app_config_t config;         // app configuration
    struct app_time_t time;             // app time structure
    char datetime[19];                  // 2021/04/02 17:01:09
    char str_ip[NET_IPV4_ADDR_LEN];     // 192.168.102.233
};

struct __attribute__((__packed__)) udp_response
{
    uint32_t ip;
    char str_ip[NET_IPV4_ADDR_LEN];
    struct app_time_t time;
    struct app_stats_t stats;
};

/*___________________________________________________________________________*/

#endif