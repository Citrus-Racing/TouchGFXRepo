#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "main.h"
#include "cmsis_os2.h"
#include "CR_CAN_parse.h"
#include "CR_structs.h"
#include "CR_encoder.h"
#include "CR_shift_light.h"
#include "CR_flash_storage.h"
extern CR_CAN_vals latest_CAN_Vals;
extern CR_button_state menu_btn_state;
extern CR_button_state back_btn_state;
extern CR_encoder_status encoder_status;
extern CR_shift_light shift_light_handle;
extern CR_encoder encoder_UI_handle;
//extern osMessageQueueId_t ADCQueueHandle;
//extern osMessageQueueId_t CANMessageQueueHandle;
Model::Model() : fuel_level_tenths(55), active_profile_index(0xFF),
                 modelListener(0),
                 menu_btn_debounce_ticks(0), back_btn_debounce_ticks(0),
                 encoder_debounce_ticks(0)
{
    // Read settings from flash.
    CR_settings_t settings;
    CR_flash_read_settings(&settings);
    fuel_level_tenths    = settings.fuel_level_tenths;
    active_profile_index = settings.active_profile_index;

    // Read all driver profiles from flash.
    CR_flash_read_all_profiles(profiles);
}

void Model::saveAll()
{
    CR_settings_t settings;
    settings.magic                = CR_SETTINGS_MAGIC;
    settings.fuel_level_tenths    = fuel_level_tenths;
    settings.active_profile_index = active_profile_index;
    CR_flash_write_all(&settings, profiles);
}

void Model::tick()
{
	// Call a function in the view that has a parameter, via the ModelListener (the active presenter).
	// The ModelListener (Presenter) has a (private) pointer reference to the active view.
	// Therefore you can only tell the view to do stuff from within a function in the actual Presenter (you cannot
	// 	do a modelListener->view->function() call in this Model.cpp file, for example)
	//
	// The view is a derivation of the ScreenViewBase class, which itself contains protected
	// member references to all of the GUI elements. You can see the .HPP file for high-level details of what to reference,
	// and you can see the .CPP file for all the gory details of config (and to help you change things)

	if(latest_CAN_Vals.CR_new_info_flag == 1){
		modelListener->update_CAN_info(&latest_CAN_Vals);
	}

	// Decrement debounce counters each tick
	if(menu_btn_debounce_ticks > 0){ menu_btn_debounce_ticks--; }
	if(back_btn_debounce_ticks > 0){ back_btn_debounce_ticks--; }
	if(encoder_debounce_ticks > 0){ encoder_debounce_ticks--; }

	if(menu_btn_state == BUTTON_PRESSED){
		menu_btn_state = BUTTON_RELEASED;
		if(menu_btn_debounce_ticks == 0){
			menu_btn_debounce_ticks = BTN_DEBOUNCE_TICKS;
			modelListener->open_menu();
		}
	}
	if(back_btn_state == BUTTON_PRESSED){
		back_btn_state = BUTTON_RELEASED;
		if(back_btn_debounce_ticks == 0){
			back_btn_debounce_ticks = BTN_DEBOUNCE_TICKS;
			modelListener->close_menu();
		}
	}

	if(encoder_UI_handle.status == ENCODER_RIGHT){
		encoder_UI_handle.status = ENCODER_STANDBY;
		modelListener->cursor_down();
	} else if(encoder_UI_handle.status == ENCODER_LEFT){
		encoder_UI_handle.status = ENCODER_STANDBY;
		modelListener->cursor_up();
	} else if(encoder_UI_handle.status == ENCODER_CLICK){
		encoder_UI_handle.status = ENCODER_STANDBY;
		if(encoder_debounce_ticks == 0){
			encoder_debounce_ticks = ENCODER_DEBOUNCE_TICKS;
			modelListener->encoder_click();
		}
	}

}
