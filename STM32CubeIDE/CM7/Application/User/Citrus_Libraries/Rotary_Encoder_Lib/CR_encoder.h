/*
 * citrus_encoder.h
 *
 *  Created on: Dec 19, 2025
 *      Author: ryanb
 */

#ifndef INC_CITRUS_ENCODER_H_
#define INC_CITRUS_ENCODER_H_

#include <stdbool.h>
#include "stm32h7xx_hal.h"
#include "CR_structs.h"


/**************************
 Encoder Status Constants
 **************************/
typedef enum {
	ENCODER_STANDBY, 	// No Detection
	ENCODER_LEFT, 		// Left Detected
    ENCODER_RIGHT, 		// Right Detected
	ENCODER_CLICK,		// Click Detected
	ENCODER_HOLD,		// Hold Detected
	ENCODER_OK,
} CR_encoder_status;


/**************************
 Encoder Data Structure

 @param turn_port1: port for encoder pin 1 	[GPIOA, GPIOD]
 @param turn_pin1: encoder pin 1 #			[GPIO_PIN_0, GPIO_PIN_15] or [(1 << 0), (1 << 15)]
 @param turn_port2: port for encoder pin 2
 @param turn_pin2: encoder pin 2 #
 @param click_port: port for encoder click
 @param click_pin: pin for encoder click

 // These are managed internally by functions; default vals false,false,true.
 @state p1_detected_first: true/false if pin1 is detected first
 @state p2_detected_first: true/false if pin2 is detected first
 @state encoder_turn_complete: true/false if both encoder lines return HI after a turn
 **************************/
typedef struct {
	GPIO_TypeDef * turn_port1;
	uint16_t turn_pin1;
	GPIO_TypeDef * turn_port2;
	uint16_t turn_pin2;

	GPIO_TypeDef * click_port;
	uint16_t click_pin;

	bool p1_detected_first;
	bool p2_detected_first;
	bool turn_finished;
	bool pin_clicked;
} CR_encoder;

CR_encoder_status CR_encoder_init(CR_encoder * handle, CR_GPIO * pin_DT, CR_GPIO * pin_CLK, CR_GPIO * pin_SW);

CR_encoder_status CR_check_encoder(CR_encoder * handle);


#endif /* INC_CITRUS_ENCODER_H_ */
