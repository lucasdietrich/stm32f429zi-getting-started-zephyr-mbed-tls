#include "hw.h"

static const struct device * dev_leds[3];
static uint8_t dev_leds_pin[3];

void hw_init_leds(void) {

    dev_leds[led_t::GREEN] = device_get_binding(LED_GREEN);
    dev_leds[led_t::BLUE] = device_get_binding(LED_BLUE);
    dev_leds[led_t::RED] = device_get_binding(LED_RED);

    dev_leds_pin[led_t::GREEN] = LED_GREEN_PIN;
    dev_leds_pin[led_t::BLUE] = LED_BLUE_PIN;
    dev_leds_pin[led_t::RED] = LED_RED_PIN;

	gpio_pin_configure(dev_leds[led_t::GREEN], LED_GREEN_PIN, GPIO_OUTPUT_ACTIVE | LED_GREEN_FLAGS);
    gpio_pin_configure(dev_leds[led_t::BLUE], LED_BLUE_PIN, GPIO_OUTPUT_ACTIVE | LED_BLUE_FLAGS);
    gpio_pin_configure(dev_leds[led_t::RED], LED_RED_PIN, GPIO_OUTPUT_ACTIVE | LED_RED_FLAGS);

    bool leds_state = 1u;

	for(uint_fast8_t i = 0; i < 10; i++)
    {
        for(uint_fast8_t led = 0; led < 3; led++)
        {
            hw_set_led((led_t) led, (led_state_t) leds_state);
        }
		leds_state = !leds_state;
        
		k_sleep(K_MSEC(100));
	}
}

void hw_set_led(led_t led, led_state_t state) {
    gpio_pin_set(dev_leds[led], dev_leds_pin[led], (int) state);
}