/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : CR_CAN_parse.h
  * Description        : Definitions for CAN message management and storage of relevant data
  ******************************************************************************
  */
/* USER CODE END Header */


#ifndef CAN_BUS_LIB_CR_CAN_PARSE_H_
#define CAN_BUS_LIB_CR_CAN_PARSE_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "main.h"
#include "fdcan.h"
#include "stdbool.h"


// This is not all the values, but it's all of them that we could ever care about
typedef struct
{
	bool		CR_new_info_flag;
	uint16_t 	engine_speed;
	uint16_t 	inlet_manifold_pressure;
	int16_t 	inlet_manifold_temperature;
	uint16_t 	throttle_position;
	int16_t 	ignition_timing;
	uint16_t 	engine_oil_pressure;
	int16_t 	boost_pressure;
	uint16_t 	boost_aim;
	uint8_t 	coolant_temperature;
	uint8_t 	engine_oil_temperature;
	uint8_t 	ecu_battery_voltage;
	uint16_t 	fuel_used;
	uint16_t 	engine_run_time;
	uint16_t 	ECU_uptime;
	int8_t 		gear;						// 4 bits
	uint8_t 	anti_lag_state;				// 4 bits
	uint8_t 	launch_state;				// 4 bits
	bool 		gear_neutral_switch;
	uint8_t 	fuel_tank_level;
	int8_t		driver_rotary_switch1;
	int8_t 		driver_rotary_switch2;
	int8_t 		driver_rotary_switch3;
	int8_t 		driver_rotary_switch4;
	int8_t 		driver_rotary_switch5;
	int8_t 		driver_rotary_switch6;
	bool 		driver_switch1;
	bool 		driver_switch2;
	bool 		driver_switch3;
	bool 		driver_switch4;
	bool 		driver_switch5;
	bool 		driver_switch6;
	bool 		driver_switch7;
	bool 		driver_switch8;
} CR_CAN_vals;

typedef struct
{
	uint16_t 	engine_speed;
	uint16_t 	engine_oil_pressure;
	uint8_t 	coolant_temperature;
	uint8_t		engine_oil_temperature;
	int8_t 		gear;						// 4 bit
	uint8_t 	anti_lag_state;				// 4 bit
	uint8_t 	launch_state;				// 4 bit
	uint8_t 	fuel_tank_level;
} CR_CAN_dash_vals;

typedef struct
{
	uint16_t	engine_speed;
	uint16_t	inlet_manifold_pressure;
	int16_t		inlet_manifold_temperature;
	uint16_t	throttle_position;
} CR_CAN_0x640;

typedef struct
{
	uint16_t	throttle_pedal;
	uint16_t 	engine_load;
	int16_t 	ignition_timing;
	int16_t		fuel_timing;
} CR_CAN_0x642;

typedef struct
{
	uint8_t		ignition_output_cut_count;
	uint8_t		fuel_output_cut_count;
	uint8_t		ignition_output_cut_average;
	uint8_t		fuel_output_cut_average;
	uint8_t		fuel_cylinder1_output_pulse_width;
	int8_t		ignition_timing_state;			// 4 bit
	int8_t		ignition_cut_request_state;		// 4 bit
	uint16_t	engine_oil_pressure;
} CR_CAN_0x644;

typedef struct
{
	int16_t		boost_pressure;
	uint16_t	boost_aim;
	uint16_t	reserved;
	uint8_t		boost_actuator_output_duty_cycle;
	int8_t		gear_level_sensor;
} CR_CAN_0x645;

typedef struct
{
	uint8_t		coolant_temperature;
	uint8_t		engine_oil_temperature;
	uint8_t		fuel_temperature;
	uint8_t		ambient_temperature;
	uint8_t		airbox_temperature;
	uint8_t		ecu_battery_voltage;
	uint16_t	fuel_used;
} CR_CAN_0x649;

typedef struct
{
	uint16_t	engine_run_time;
	uint16_t	ECU_uptime;
	uint8_t		warning_source;
	bool		coolant_temperature_warning;
	bool		coolant_pressure_warning;
	bool		engine_speed_warning;
	bool		engine_oil_temperature_warning;
	bool		engine_oil_pressure_warning;
	bool		reserved;
	bool		crankcase_pressure_warning;
	bool		fuel_pressure_warning;
	bool		knock_warning;
} CR_CAN_0x64C;

typedef struct
{
	uint8_t		engine_state;					// 4 bit
	uint8_t		fuel_pump_state;				// 4 bit
	uint8_t		anti_lag_state;					// 4 bit
	uint8_t		launch_state;					// 4 bit
	uint8_t		boost_aim_state;				// 4 bit
	uint8_t		engine_speed_limit_state;	    // 4 bit
	uint8_t		engine_overrun_state;			// 4 bit
	uint8_t		fuel_cut_state;					// 4 bit
	uint8_t		fuel_purge_state;				// 4 bit
	uint8_t		knock_state;					// 4 bit
	uint8_t		throttle_aim_state;				// 4 bit
	uint8_t		fuel_closed_loop_state;			// 4 bit
	uint8_t		engine_speed_reference_state;   // 4 bit
	int8_t		gear;							// 4 bit
	uint8_t		reserved;
} CR_CAN_0x64D;

typedef struct
{
	int8_t		anti_lag_diagnostic;   			// 4 bit
	int8_t		launch_diagnostic;				// 4 bit
	int8_t		boost_control_diagnostic;		// 4 bit
	int8_t		fuel_cut_state;					// 4 bit
	int8_t		fuel_close_loop_diagnostic;		// 4 bit
	int8_t		reserved;						// 4 bit
	bool		engine_oil_pressure_low_switch;
	bool		pit_switch;
	bool		launch_enable_switch;
	bool		traction_enable_switch;
	bool		brake_switch;
	bool		anti_lag_switch;
	bool		engine_run_switch;
	bool		gear_neutral_switch;
} CR_CAN_0x64E;

typedef struct
{
	uint16_t	engine_run_time_total;
	uint8_t		fuel_closed_loop_control_bank1_trim;
	uint8_t		fuel_closed_loop_control_bank2_trim;
	uint8_t		gearbox_temperature;
	uint8_t		fuel_tank_level;
	int16_t		reserved;
} CR_CAN_0x64F;

typedef struct
{
	int8_t		driver_rotary_switch1;
	int8_t		driver_rotary_switch2;
	int8_t		driver_rotary_switch3;
	int8_t		driver_rotary_switch4;
	int8_t		driver_rotary_switch5;
	int8_t		driver_rotary_switch6;
	int8_t		reserved;
	bool		driver_switch1;
	bool		driver_switch2;
	bool		driver_switch3;
	bool		driver_switch4;
	bool		driver_switch5;
	bool		driver_switch6;
	bool		driver_switch7;
	bool		driver_switch8;
} CR_CAN_0x650;

typedef struct
{
	int16_t fuel_injector_primary_pressure;
	int16_t fuel_injector_secondary_pressure;
	uint16_t gear_input_shaft_speed;
	uint16_t gear_output_shaft_speed;
} CR_CAN_0x657;


// These are the user-facing master functions
void CR_parse_CAN(CR_CAN_vals * data_handle, FDCAN_HandleTypeDef* hfdcan, uint32_t FDCAN_RX_FIFOx, FDCAN_RxHeaderTypeDef* CAN_RX_info_handle);
//void LoadDashDisplayStruct(DashboardDisplayValuesTypeDef* dash);

// Helper functions to populate the central struct
void parse_0x640(CR_CAN_vals * data_handle, uint8_t * CAN_msg);
//CR_CAN_0x640 CR_parse_0x642(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x644 CR_parse_0x644(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x645 CR_parse_0x645(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x649 CR_parse_0x649(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x64C CR_parse_0x64C(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x64D CR_parse_0x64D(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x64E CR_parse_0x64E(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x64F CR_parse_0x64F(FDCAN_HandleTypeDef* hfdcan);
//CR_CAN_0x650 CR_parse_0x650(FDCAN_HandleTypeDef* hfdcan);

#endif /* CAN_BUS_LIB_CR_CAN_PARSE_H_ */

