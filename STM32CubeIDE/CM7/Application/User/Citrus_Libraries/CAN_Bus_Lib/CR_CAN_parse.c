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
 * 	and if there is a message to read, it will read it and attempt to parse. Messages
 * 	That are not part of the datastructure will be discarded.
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
	}
}


// DELETE this one!!!
//void LoadDashDisplayStruct(DashboardDisplayValuesTypeDef* dash)
//{
//
//	dash->EngineSpeed = CurrentECUValues.EngineSpeed;
//	dash->EngineOilPressure = CurrentECUValues.EngineOilPressure;
//	dash->CoolantTemperature = CurrentECUValues.CoolantTemperature;
//	dash->EngineOilTemperature = CurrentECUValues.EngineOilTemperature;
//	dash->Gear = CurrentECUValues.Gear;
//	dash->AntiLagState = CurrentECUValues.AntiLagState;
//	dash->LaunchState = CurrentECUValues.LaunchState;
//	dash->FuelTankLevel = CurrentECUValues.FuelTankLevel;
//
//	return *dash;
//}

// Populates the data handle with 0x640 data.
// This function assumes an 8-byte CAN message that is indeed 0x640.
void parse_0x640(CR_CAN_vals * data_handle, uint8_t * CAN_msg)
{
	data_handle->engine_speed = ((uint16_t)CAN_msg[0] << 8) | CAN_msg[1];
	data_handle->inlet_manifold_pressure = ((uint16_t)CAN_msg[2] << 8) | CAN_msg[3];
	data_handle->inlet_manifold_temperature = ((uint16_t)CAN_msg[4] << 8) | CAN_msg[5];
	data_handle->throttle_position = ((uint16_t)CAN_msg[6] << 8) | CAN_msg[7];
}
//
//CANx642TypeDef Parsex642(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER1, &CAN_RX_Config, CAN_read_buff);
//	CANx642TypeDef buf = {0};
//	buf.ThrottlePedal = ((uint16_t)CAN_read_buff[0] << 8) | CAN_read_buff[1];
//	buf.EngineLoad = ((uint16_t)CAN_read_buff[3] << 8) | CAN_read_buff[4];
//	buf.IgnitionTiming = ((uint16_t)CAN_read_buff[5] << 8) | CAN_read_buff[6];
//	buf.FuelTiming = ((uint16_t)CAN_read_buff[7] << 8) | CAN_read_buff[8];
//	return buf;
//}
//
//CANx644TypeDef Parsex644(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER2, &CAN_RX_Config, CAN_read_buff);
//	CANx644TypeDef buf = {0};
//	buf.IgnitionOutputCutCount = CAN_read_buff[0];
//	buf.FuelOutputCutCount = CAN_read_buff[1];
//	buf.IgnitionOutputCutAverage = CAN_read_buff[2];
//	buf.FuelOutputCutAverage = CAN_read_buff[3];
//	buf.FuelCylinder1OutputPulseWidth = CAN_read_buff[4];
//	buf.IgnitionTimingState = ((int8_t)CAN_read_buff[5] & 0x0F);
//	buf.IgnitionCutRequestState = ((int8_t)CAN_read_buff[5] & 0xF0) >> 4;
//	buf.EngineOilPressure = ((uint16_t)CAN_read_buff[6] << 8) | CAN_read_buff[7];
//	return buf;
//}
//
//CANx645TypeDef Parsex645(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER3, &CAN_RX_Config, CAN_read_buff);
//	CANx645TypeDef buf = {0};
//	buf.BoostPressure = ((int16_t)CAN_read_buff[0] << 8) | CAN_read_buff[1];
//	buf.BoostAim = ((uint16_t)CAN_read_buff[2] << 8) | CAN_read_buff[3];
//	buf.BoostActuatorOutputDutyCycle = CAN_read_buff[6];
//	buf.GearLeverSensor = CAN_read_buff[7];
//	return buf;
//}
//
//CANx649TypeDef Parsex649(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER4, &CAN_RX_Config, CAN_read_buff);
//	CANx649TypeDef buf = {0};
//	buf.CoolantTemperature = CAN_read_buff[0];
//	buf.EngineOilTemperature = CAN_read_buff[1];
//	buf.FuelTemperature = CAN_read_buff[2];
//	buf.AmbientTemperature = CAN_read_buff[3];
//	buf.AirboxTemperature = CAN_read_buff[4];
//	buf.ECUBatteryVoltage = CAN_read_buff[5];
//	buf.FuelUsed = ((uint16_t)CAN_read_buff[6] << 8) | CAN_read_buff[7];
//	return buf;
//}
//
//CANx64CTypeDef Parsex64C(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER5, &CAN_RX_Config, CAN_read_buff);
//	CANx64CTypeDef buf = {0};
//	buf.EngineRunTime = ((uint16_t)CAN_read_buff[0] << 8) | CAN_read_buff[1];
//	buf.ECUUptime = ((uint16_t)CAN_read_buff[2] << 8) | CAN_read_buff[3];
//	buf.WarningSource = CAN_read_buff[4];
//	buf.CoolantTemperatureWarning = (bool)(CAN_read_buff[5] >> 0) & 1;
//	buf.CoolantPressureWarning = (bool)(CAN_read_buff[5] >> 1) & 1;
//	buf.EngineSpeedWarning = (bool)(CAN_read_buff[5] >> 2) & 1;
//	buf.EngineOilTemperatureWarning = (bool)(CAN_read_buff[5] >> 3) & 1;
//	buf.EngineOilPressureWarning = (bool)(CAN_read_buff[5] >> 4) & 1;
//	buf.CrankcasePressureWarning = (bool)(CAN_read_buff[5] >> 6) & 1;
//	buf.FuelPressureWarning = (bool)(CAN_read_buff[5] >> 7) & 1;
//	buf.KnockWarning = (bool)(CAN_read_buff[6] >> 0) & 1;
//	return buf;
//}
//
//CANx64DTypeDef Parsex64D(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER6, &CAN_RX_Config, CAN_read_buff);
//	CANx64DTypeDef buf = {0};
//	buf.EngineState = (CAN_read_buff[0] & 0x0F);
//	buf.FuelPumpState = (CAN_read_buff[0] & 0xF0) >> 4;
//	buf.AntiLagState = (CAN_read_buff[1] & 0x0F);
//	buf.LaunchState = (CAN_read_buff[1] & 0xF0) >> 4;
//	buf.BoostAimState = (CAN_read_buff[2] & 0x0F);
//	buf.EngineSpeedLimitState = (CAN_read_buff[2] & 0xF0) >> 4;
//	buf.EngineOverrunState = (CAN_read_buff[3] & 0x0F);
//	buf.FuelCutState = (CAN_read_buff[3] & 0xF0) >> 4;
//	buf.FuelPurgeState = (CAN_read_buff[4] & 0x0F);
//	buf.ThrottleAimState = (CAN_read_buff[4] & 0xF0) >> 4;
//	buf.FuelClosedLoopState = (CAN_read_buff[5] & 0x0F);
//	buf.EngineSpeedReferenceState = (CAN_read_buff[5] & 0xF0) >> 4;
//	buf.Gear = ((int8_t)CAN_read_buff[6] & 0x0F);
//	return buf;
//}
//
//CANx64ETypeDef Parsex64E(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER7, &CAN_RX_Config, CAN_read_buff);
//	CANx64ETypeDef buf = {0};
//	buf.AntiLagDiagnostic = ((int8_t)CAN_read_buff[0] & 0x0F);
//	buf.LaunchDiagnostic = ((int8_t)CAN_read_buff[0] & 0xF0) >> 4;
//	buf.BoostControlDiagnostic = ((int8_t)CAN_read_buff[1] & 0x0F);
//	buf.FuelCutState = ((int8_t)CAN_read_buff[1] & 0xF0) >> 4;
//	buf.FuelClosedLoopDiagnostic = ((int8_t)CAN_read_buff[2] & 0x0F);
//	buf.EngineOilPressureLowSwitch = (bool)(CAN_read_buff[3] >> 0) & 1;
//	buf.PitSwitch = (bool)(CAN_read_buff[5] >> 1) & 1;
//	buf.LaunchEnableSwitch = (bool)(CAN_read_buff[5] >> 2) & 1;
//	buf.TractionEnableSwitch = (bool)(CAN_read_buff[5] >> 3) & 1;
//	buf.BrakeSwitch = (bool)(CAN_read_buff[5] >> 4) & 1;
//	buf.AntiLagSwitch = (bool)(CAN_read_buff[6] >> 5) & 1;
//	buf.EngineRunSwitch = (bool)(CAN_read_buff[5] >> 6) & 1;
//	buf.GearNeutralSwitch = (bool)(CAN_read_buff[5] >> 7) & 1;
//	return buf;
//}
//
//CANx64FTypeDef Parsex64F(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER8, &CAN_RX_Config, CAN_read_buff);
//	CANx64FTypeDef buf = {0};
//	buf.EngineRunTimeTotal = ((uint16_t)CAN_read_buff[0] << 8) | CAN_read_buff[1];
//	buf.FuelClosedLoopControlBank1Trim = CAN_read_buff[1];
//	buf.FuelClosedLoopControlBank2Trim = CAN_read_buff[2];
//	buf.GearboxTemperature = CAN_read_buff[3];
//	buf.FuelTankLevel = CAN_read_buff[4];
//	return buf;
//}
//
//CANx650TypeDef Parsex650(FDCAN_HandleTypeDef* hfdcan)
//{
//	FDCAN_RxHeaderTypeDef CAN_RX_Config;
//	uint8_t CAN_read_buff[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
//	HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_BUFFER9, &CAN_RX_Config, CAN_read_buff);
//	CANx650TypeDef buf = {0};
//	buf.DriverRotarySwitch1 = (int8_t)CAN_read_buff[0];
//	buf.DriverRotarySwitch2 = (int8_t)CAN_read_buff[1];
//	buf.DriverRotarySwitch3 = (int8_t)CAN_read_buff[2];
//	buf.DriverRotarySwitch4 = (int8_t)CAN_read_buff[3];
//	buf.DriverRotarySwitch5 = (int8_t)CAN_read_buff[4];
//	buf.DriverRotarySwitch6 = (int8_t)CAN_read_buff[5];
//	buf.DriverSwitch1 = (bool)(CAN_read_buff[7] >> 0) & 1;
//	buf.DriverSwitch2 = (bool)(CAN_read_buff[7] >> 1) & 1;
//	buf.DriverSwitch3 = (bool)(CAN_read_buff[7] >> 2) & 1;
//	buf.DriverSwitch4 = (bool)(CAN_read_buff[7] >> 3) & 1;
//	buf.DriverSwitch5 = (bool)(CAN_read_buff[7] >> 4) & 1;
//	buf.DriverSwitch6 = (bool)(CAN_read_buff[7] >> 5) & 1;
//	buf.DriverSwitch7 = (bool)(CAN_read_buff[7] >> 6) & 1;
//	buf.DriverSwitch8 = (bool)(CAN_read_buff[7] >> 7) & 1;
//	return buf;
//}
