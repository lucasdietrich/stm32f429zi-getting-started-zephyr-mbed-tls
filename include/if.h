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

void init_if(void);

void down_if(void);

#endif