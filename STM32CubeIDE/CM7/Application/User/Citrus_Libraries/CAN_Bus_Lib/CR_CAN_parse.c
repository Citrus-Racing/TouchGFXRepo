#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "fdcan.h"
#include "stdbool.h"
#include "CR_CAN_parse.h"


// This holds all values from the CAN bus that can be directly read to update the dash display.
CR_CAN_vals latest_CAN_Vals = {0};


// **************
// User Functions
// **************

/*  Call this function to check if CAN mailbox (HAL_FDCAN_GetRxFifoFillLevel) > 1,
 * 	and if there is a message to read, it will read it and attempt to parse.
 *
 * 	We only store a sub-set of the total message sent by the ECU.
 * 	data that is not part of the datastructure will be discarded.
 * 	This function calls many sub-parse 'private' functions.
 *
 *  @param data_handle the handle to store updated CAN values computed from this function
 *  @param hfdcan the active CAN bus handle
 *  @param FDCAN_RX_FIFOx the FIFO to process data from
 *  @param CAN_RX_info_handle the handle to populate can header data from
 *
 */
void CR_parse_CAN(CR_CAN_vals * data_handle, FDCAN_HandleTypeDef* hfdcan, uint32_t FDCAN_RX_FIFOx, FDCAN_RxHeaderTypeDef* CAN_RX_info_handle)
{
	if (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFOx) > 1){
		data_handle->CR_new_info_flag = 1; // let GUI know that there's new data.
		uint8_t CAN_read_buff[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}; // Initialize with obvious debugging values.
		HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFOx, CAN_RX_info_handle, CAN_read_buff);

		// Parse the data differently depending on the ID, according to M1 Tune
		if(CAN_RX_info_handle->Identifier == 0x640) { parse_0x640(data_handle, CAN_read_buff); }
		if(CAN_RX_info_handle->Identifier == 0x649) { parse_0x649(data_handle, CAN_read_buff); }
		if(CAN_RX_info_handle->Identifier == 0x64D) { parse_0x64D(data_handle, CAN_read_buff); }
		if(CAN_RX_info_handle->Identifier == 0x657) { parse_0x657(data_handle, CAN_read_buff); }
	}
}


// ************************************
// Data Population 'Private' Functions
// ************************************
// Populates the data handle with 0x640 data.
// This function assumes an 8-byte CAN message that is indeed 0x640.
void parse_0x640(CR_CAN_vals * data_handle, uint8_t * CAN_msg) {
	data_handle->engine_speed = ((uint16_t)CAN_msg[0] << 8) | CAN_msg[1];
	data_handle->inlet_manifold_pressure = ((uint16_t)CAN_msg[2] << 8) | CAN_msg[3];
	data_handle->inlet_manifold_temperature = ((uint16_t)CAN_msg[4] << 8) | CAN_msg[5];
	data_handle->throttle_position = ((uint16_t)CAN_msg[6] << 8) | CAN_msg[7];
}

void parse_0x649(CR_CAN_vals * data_handle, uint8_t * CAN_msg) {
	data_handle->coolant_temperature = CAN_msg[0];
	data_handle->engine_oil_temperature = CAN_msg[1];
	// discard the rest of the data transmission
}

void parse_0x64D(CR_CAN_vals * data_handle, uint8_t * CAN_msg) {
	data_handle->anti_lag_state = (uint8_t)(CAN_msg[1] >> 4);
	data_handle->launch_state = (uint8_t)(CAN_msg[1] & 0x0F);
	data_handle->gear = (int8_t)(CAN_msg[6] & 0x0F);
	// discard the rest of the data transmission
}

void parse_0x657(CR_CAN_vals * data_handle, uint8_t * CAN_msg){
	data_handle->gear_output_shaft_speed = ((uint16_t)CAN_msg[6] << 8) | CAN_msg[7];
}

