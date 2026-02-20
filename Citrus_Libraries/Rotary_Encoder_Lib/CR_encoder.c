/*
 * citrus_encoder.c
 *
 *  Created on: Dec 19, 2025
 *      Author: RyBry
 */

#include "CR_encoder.h"

// Configure the pins correctly as inputs beforehand in CubeMX.
CR_encoder_status CR_encoder_init(CR_encoder * handle, CR_GPIO * pin_DT, CR_GPIO * pin_CLK, CR_GPIO * pin_SW){
	// Store user provided values
	handle->turn_port1 = pin_DT->GPIO_Port;
	handle->turn_pin1 = pin_DT->GPIO_Pin;
	handle->turn_port2 = pin_CLK->GPIO_Port;
	handle->turn_pin2 = pin_CLK->GPIO_Pin;
	handle->click_port = pin_SW->GPIO_Port;
	handle->click_pin = pin_SW->GPIO_Pin;
	// Set default values.
	handle->p1_detected_first = false;
	handle->p2_detected_first = false;
	handle->turn_finished = true;

	return ENCODER_OK;
}

// This function assumes encoder snaps from HI -> LOW on a turn.
// User must manually reset the encoder status to standby to acknowledge the turn.
void CR_check_encoder(CR_encoder * handle){
	GPIO_PinState pin1_val = HAL_GPIO_ReadPin(handle->turn_port1, handle->turn_pin1);
	GPIO_PinState pin2_val = HAL_GPIO_ReadPin(handle->turn_port2, handle->turn_pin2);
	GPIO_PinState click_pin_val = HAL_GPIO_ReadPin(handle->click_port, handle->click_pin);
	// Turning Logic
	if (pin1_val && pin2_val){ // if both lines are high - ready to make another read. This also eliminates halfway rotation issues.
		handle->p1_detected_first = false;
		handle->p2_detected_first = false;
		handle->turn_finished = true;
	}
	else if (handle->turn_finished && pin1_val && !pin2_val){  // if one is high and one is low... then a turn has been started
		handle->p2_detected_first = true;
		handle->turn_finished = false;
	} else if (handle->turn_finished && !pin1_val && pin2_val){
		handle->p1_detected_first = true;
		handle->turn_finished = false;
	}
	else if(handle->p1_detected_first && !pin2_val){   // If one pin already went low, and now the other is low... Turn one direction
		// reset and register the turn
		handle->p1_detected_first = false;
		handle->status = ENCODER_LEFT;
	}
	else if (handle->p2_detected_first && !pin1_val){   // Turn other direction
		// reset and register the turn
		handle->p2_detected_first = false;
		handle->status = ENCODER_RIGHT;
	}

	// Click is an active low w/ a pullup resistor
	if (click_pin_val == 0){
		handle->status = ENCODER_CLICK;
	}
}
