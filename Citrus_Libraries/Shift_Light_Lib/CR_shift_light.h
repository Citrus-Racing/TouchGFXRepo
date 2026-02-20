/*
 * CR_shift_light.h
 *
 */

#ifndef SHIFT_LIGHT_LIB_CR_SHIFT_LIGHT_H_
#define SHIFT_LIGHT_LIB_CR_SHIFT_LIGHT_H_

#include "main.h"
#include <stdlib.h>

typedef enum {
	CR_OK = 1,
	CR_ERROR = 2,
} CR_shift_light_status;

// This struct should be built by calling the C constructor below.
// Some properties are assigned by user, while others are computed in the constructor.
typedef struct {
    TIM_HandleTypeDef *timer;               // Assigned; 800 kHz PWM channel timer.
    uint32_t channel;						// Assigned; Timer channel (use STM32 defines).
	uint8_t num_leds;						// Assigned; The number of LEDs.
    uint16_t ticks_per_PWM_cycle;		    // Computed; Given timer clock speed, we need to know how many ticks we have in a PWM cycle for 800Khz (1.25 microseconds period).
    uint32_t ticks_HI_per_1;				// Computed; should be 64% of ticks_per_PWM_cycle as per ws2812 datasheet
    uint32_t ticks_HI_per_0;				// Computed; should should be 32% of ticks_per_PWM_cycle as per ws2812 datasheet
    uint32_t pulses_LO_per_RESET;			// Assigned; Should be #pulses equivalent to 50 microseconds of low time, as per ws2812 datasheet
    uint16_t total_pulses;					// Computed; the total pulses that will be sent for this LED strip config.
    uint32_t * dma_transfer_buff;			// Computed; We're using a 32 bit timer. This is a Pointer to malloc'd DMA transfer buffer.
    float brightness;				        // Value 0.0-1.0 scalar.
}  CR_shift_light;

CR_shift_light_status CR_shift_light_init(CR_shift_light * handle, TIM_HandleTypeDef *timer, uint16_t ticks_per_PWM_cycle, uint32_t channel, uint8_t num_leds);

CR_shift_light_status CR_shift_light_deinit(CR_shift_light * handle);

CR_shift_light_status CR_set_all_lights(CR_shift_light * handle, uint8_t r, uint8_t g, uint8_t b);

CR_shift_light_status CR_set_brightness(CR_shift_light * handle, float brightness);

// Set all the lights to a value
CR_shift_light_status CR_set_light(CR_shift_light * handle, uint8_t led_num, uint8_t r, uint8_t g, uint8_t b);

// Turn off all the lights
CR_shift_light_status CR_clear_all_lights(CR_shift_light * handle);

// Turns one LED on and then off, moves to next LED and does the same. Move through the entire strip.
CR_shift_light_status CR_cascade_line_blink(CR_shift_light * handle, uint8_t r, uint8_t g, uint8_t b);

// An assortment of flash types to test LED system.
CR_shift_light_status CR_Test_Sequence_Flash(CR_shift_light * handle);

#endif /* SHIFT_LIGHT_LIB_CR_SHIFT_LIGHT_H_ */
