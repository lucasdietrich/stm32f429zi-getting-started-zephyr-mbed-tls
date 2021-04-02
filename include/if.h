/**
 * @file if.h
 * @author Lucas Dietrich (pro@ldietrich.fr)
 * @brief Initialize interfaces
 *
 *
 * @version 0.1
 * @date 2021-03-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _IF_H
#define _IF_H

#include <net/net_if.h>
#include <net/net_core.h>
#include <net/net_context.h>
#include <net/net_mgmt.h>
#include <net/net_config.h>

#include <net/sntp.h>

// #include <sys/time.h>

void init_if(void);

void down_if(void);

// get IP functions
uint32_t get_ip(void);

int get_ip(char * buffer, unsigned int buffer_len);

/**
 * @brief Get seconds from the the sntp time object
 * 
 * @return uint64_t : current time in seconds, return 0 if error
 */
uint64_t get_sntp_time(void);

#endif