#include "hw.h"

/*___________________________________________________________________________*/

static struct led_t leds[3];

/*___________________________________________________________________________*/

void hw_init_leds(void) {
    leds[GREEN].dev = device_get_binding(LED_GREEN_PORT);
    leds[GREEN].pin = LED_GREEN_PIN;

    leds[BLUE].dev = device_get_binding(LED_BLUE_PORT);
    leds[BLUE].pin = LED_BLUE_PIN;

    leds[RED].dev = device_get_binding(LED_RED_PORT);
    leds[RED].pin = LED_RED_PIN;

	gpio_pin_configure(leds[GREEN].dev, LED_GREEN_PIN, GPIO_OUTPUT_ACTIVE | LED_GREEN_FLAGS);
    gpio_pin_configure(leds[BLUE].dev, LED_BLUE_PIN, GPIO_OUTPUT_ACTIVE | LED_BLUE_FLAGS);
    gpio_pin_configure(leds[RED].dev, LED_RED_PIN, GPIO_OUTPUT_ACTIVE | LED_RED_FLAGS);

    bool leds_state = true;

	for(uint_fast8_t i = 0; i < 10; i++)
    {
        for(uint_fast8_t led = 0; led < 3; led++)
        {
            hw_set_led((led_e) led, (led_state_t) leds_state);
        }
		leds_state = !leds_state;
        
		k_sleep(K_MSEC(100));
	}
}
void hw_set_led(led_e led, led_state_t state)
{
    led_t * L = &leds[(uint32_t) led];

    hw_set_led(L, state);
}

void hw_set_led(led_t *led, led_state_t state)
{
    gpio_pin_set(led->dev, led->pin, (int) state);
}