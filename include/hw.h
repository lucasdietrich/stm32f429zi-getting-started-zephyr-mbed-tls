/**
 * @file hw.h
 * @author Lucas Dietrich (pro@ldietrich.fr)
 * @brief Initialize LEDS
 *
 *
 * @version 0.1
 * @date 2021-03-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _HW_H
#define _HW_H

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

/*___________________________________________________________________________*/

#define LED_GREEN_NODE DT_ALIAS(led0)

// GREEN LED
#if DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
#define LED_GREEN	            DT_GPIO_LABEL(LED_GREEN_NODE, gpios)
#define LED_GREEN_PIN	        DT_GPIO_PIN(LED_GREEN_NODE, gpios)
#define LED_GREEN_FLAGS	        DT_GPIO_FLAGS(LED_GREEN_NODE, gpios)
#else

/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#define LED_BLUE_NODE DT_ALIAS(led1)

// BLUE LED
#if DT_NODE_HAS_STATUS(LED_BLUE_NODE, okay)
#define LED_BLUE	            DT_GPIO_LABEL(LED_BLUE_NODE, gpios)
#define LED_BLUE_PIN	        DT_GPIO_PIN(LED_BLUE_NODE, gpios)
#define LED_BLUE_FLAGS	        DT_GPIO_FLAGS(LED_BLUE_NODE, gpios)
#else

/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

#define LED_RED_NODE DT_ALIAS(led2)

// RED LED
#if DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
#define LED_RED	                DT_GPIO_LABEL(LED_RED_NODE, gpios)
#define LED_RED_PIN	            DT_GPIO_PIN(LED_RED_NODE, gpios)
#define LED_RED_FLAGS	        DT_GPIO_FLAGS(LED_RED_NODE, gpios)
#else

/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led2 devicetree alias is not defined"
#endif

/*___________________________________________________________________________*/

/**
 * Types
 */
enum led_t { GREEN, BLUE, RED};

/*
led_t& operator++(led_t& led)
{
    // https://stackoverflow.com/a/15456011/15309977
    switch(led) {
        case led_t::GREEN: return led = led_t::BLUE;
        case led_t::BLUE: return led = led_t::RED;
        case led_t::RED: return led = led_t::GREEN;
    }
}
*/

enum led_state_t { OFF, ON};

/*
led_state_t& operator!(led_state_t &state)
{
    switch(state) {
        case led_state_t::OFF: return state = led_state_t::ON;
        case led_state_t::ON: return state = led_state_t::OFF;
    }
}
*/

/*___________________________________________________________________________*/

/**
 * @brief Initialize leds (green, blue, red) and blink them during 1 second
 */
void hw_init_leds(void);

void hw_set_led(led_t led, led_state_t state);


#endif