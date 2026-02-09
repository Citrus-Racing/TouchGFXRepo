/*
 * citrus_shift_light.c
 *
 * Implementation file for PWM controlled WS2812B single data line LEDs
 *
 * This library assumes a 32-bit PWM timer channel is enabled for full Word-sized DMA circular transfers
 *
 * Functions meant to be called by the end-user are pre-fixed with "CR".
 *
 * Beware of this confusion: Every LED requires 24 pulses to be sent. This is
 * transmitted via PWM, and the duty cycle is an 32-bit (4 byte) number. Therefore, since we need to send 24 pulses,
 * we really need to transmit 4*24 = 96 BYTES to the DMA for one LED signal to be complete,
 * even though you may be inclined to think of each pulse as a single bit when looking at the oscilloscope.
 *
 */

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "CR_shift_light.h"


// *************
// Configuration
// *************

/*
 * Initalization function
 *
 * @param handle the reference to the shift light structure
 * @param timer the PWM timer that is being used to send data (ex: &htimx)
 * @param timer_CLK_MHz the timer clock rate supplied to the timer after prescaler (ex 100Mhz clock in with timer prescaler '2': 50)
 * @param channel the timer channel used for PWM output (ex: TIM_CHANNEL_x)
 * @param num_leds the number of leds in the strip
 */
CR_shift_light_status CR_shift_light_init(CR_shift_light * handle, TIM_HandleTypeDef *timer, uint16_t ticks_per_PWM_cycle, uint32_t channel, uint8_t num_leds){

	handle->timer = timer;
	handle->channel = channel;
	handle->num_leds = num_leds;
	handle->ticks_per_PWM_cycle = ticks_per_PWM_cycle; // For example: [(timer clock 240MHz) / 800KHz required LED bitrate] = 300 ticks per PWM cycle; Make sure the time is also properly configured to this period!
	handle->ticks_HI_per_1 = (uint32_t) (0.64 * handle->ticks_per_PWM_cycle);
	handle->ticks_HI_per_0 = (uint32_t) (0.32 * handle->ticks_per_PWM_cycle);
	handle->pulses_LO_per_RESET = 50; // at 800Khz, a pulse is sent every 1.25 us, and reset signal requires at least 50 micros low. So there must be >= 40 pulse times of zero voltage. Assigning easy constant 50.
	handle->total_pulses = (24*handle->num_leds) + (handle->pulses_LO_per_RESET);
	handle->dma_transfer_buff = calloc(handle->total_pulses, sizeof(*(handle->dma_transfer_buff))); // clear allocate memory to all zeros. This naturally creates the zero reset of 50 pulses at the end.
    HAL_TIM_PWM_Start_DMA(handle->timer, handle->channel, handle->dma_transfer_buff, handle->total_pulses);

	return CR_OK;
}


// De-init Function. Stop DMA timer and free memory
CR_shift_light_status CR_shift_light_deinit(CR_shift_light * handle){
	HAL_TIM_PWM_Stop_DMA(handle->timer, handle->channel);
	free(handle->dma_transfer_buff);
	return CR_OK;
}

CR_shift_light_status CR_set_brightness(CR_shift_light * handle, float brightness){
	if (brightness > 1){
		return CR_ERROR;
	}
	if (brightness < 0.1){
		brightness = 0;
	}
	handle->brightness = brightness;
	return CR_OK;
}


// ****************************
// Core Manual Control Functions
// *****************************
// Note: we do not synchronize the DMA using PWM callbacks.
// With 7 LEDs, the DMA transfer into the buffer takes ~30 micros to write,
// while the DMA PWM gen loop takes ~270 micros to finish. There will be a mix
// of partial new and old values for the first loop. However, this is fully
// rectified by the next loop, and at a refresh rate of 3.7KHz, the blip is well beyond visual perception.


// Set all the lights
CR_shift_light_status CR_set_all_lights(CR_shift_light * handle, uint8_t r, uint8_t g, uint8_t b) {
	uint8_t s_r = handle->brightness * r;
	uint8_t s_g = handle->brightness * g;
	uint8_t s_b = handle->brightness * b;
	// Outer loop selects a single LED sequence transfer of 96 bytes (24 values, 4 bytes per value).
	// variable i starts at the least significant position of a 24 value sequence within dma_transfer_buff.
	for(int i = 0; i < (24*handle->num_leds); i += 24){ // i is the bit position of the first transferred item of this 24-bit block. i+1 is the 2nd bit transferred.
		// Write all values for one LED (24 values, 8 values per LED, 96 bytes) using bitwise AND extraction from rgb.
		// Color order to send is: G, R, B. Send most significant bit of each LED color first.
		for(int j = 0; j < 8; j++){
			// Start by assigning the least significant bit in the buffer to the most significant bit in the byte color LED
			handle->dma_transfer_buff[i+j] = s_g&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;       // bits [0,7]
			handle->dma_transfer_buff[i+j+8] = s_r&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;     // bits [8,15]
			handle->dma_transfer_buff[i+j+16] = s_b&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;    // bits [16,31]
		}
	}
	return CR_OK;
}


// Set a single light to a desired color.
CR_shift_light_status CR_set_light(CR_shift_light * handle, uint8_t led_num, uint8_t r, uint8_t g, uint8_t b) {
	uint8_t s_r = handle->brightness * r;
	uint8_t s_g = handle->brightness * g;
	uint8_t s_b = handle->brightness * b;
	uint8_t i = led_num*24;
		for(int j = 0; j < 8; j++){
			handle->dma_transfer_buff[i+j] = s_g&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;       // bits [0,7]
			handle->dma_transfer_buff[i+j+8] = s_r&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;     // bits [8,15]
			handle->dma_transfer_buff[i+j+16] = s_b&(1<<(7-j)) ? handle->ticks_HI_per_1 : handle->ticks_HI_per_0;    // bits [16,31]
		}
		return CR_OK;
}


// Turn all of the lights off
CR_shift_light_status CR_clear_all_lights(CR_shift_light * handle) {
	for(int i = 0; i < (24*handle->num_leds); i += 24){
		for(int j = 0; j < 8; j++){
			handle->dma_transfer_buff[i+j] = handle->ticks_HI_per_0;       // bits [0, 7]
			handle->dma_transfer_buff[i+j+8] = handle->ticks_HI_per_0;     // bits [8,15]
			handle->dma_transfer_buff[i+j+16] = handle->ticks_HI_per_0;    // bits [16,31]
		}
	}
	return CR_OK;
}



// *******************
// Pre-made Animations
// *******************

CR_shift_light_status CR_cascade_line_blink(CR_shift_light * handle, uint8_t r, uint8_t g, uint8_t b){
	CR_clear_all_lights(handle);
	for(int i = 0; i < handle->num_leds; i++){
		CR_set_light(handle, i, r, g, b); //green, blue, red
		//setLedValues(ws, i, g, r, b);
		osDelay(50);
		CR_set_light(handle, i, 0, 0, 0);
	}
	return CR_OK;
}


// An assortment of flash types to test LED system.
CR_shift_light_status CR_Test_Sequence_Flash(CR_shift_light * handle){
	// Red all HI, LOW gradient
	for(int i = 0; i < 255; i++){
		CR_set_all_lights(handle, i, 0, 0);
		osDelay(5);
	}
	for(int i = 255; i >= 0; i--){
		CR_set_all_lights(handle, i, 0, 0);
		osDelay(5);
	}
	// Blue all HI, LOW gradient
	for(int i = 0; i < 255; i++){
		CR_set_all_lights(handle, 0, i, 0);
		osDelay(5);
	}
	for(int i = 255; i >= 0; i--){
		CR_set_all_lights(handle, 0, i, 0);
		osDelay(5);
	}
	// Green all HI, LOW gradient
	for(int i = 0; i < 255; i++){
		CR_set_all_lights(handle, 0, 0, i);
		osDelay(5);
	}
	for(int i = 255; i >= 0; i--){
		CR_set_all_lights(handle, 0, 0, i);
		osDelay(5);
	}
	// Arbitrary light colors all on
	CR_set_all_lights(handle, 255, 255, 255);
	osDelay(500);
	CR_set_all_lights(handle, 230, 100, 50);
	osDelay(500);
	CR_set_all_lights(handle, 50, 255, 100);
	osDelay(500);
	CR_set_all_lights(handle, 50, 200, 50);
	osDelay(500);

	// One LED on at a time and travels through linen
	CR_cascade_line_blink(handle, 230, 150, 0);
	osDelay(50);
	CR_cascade_line_blink(handle, 0, 255, 150);
	osDelay(50);
	CR_cascade_line_blink(handle, 150, 0, 255);
	osDelay(50);

	return CR_OK;
}


