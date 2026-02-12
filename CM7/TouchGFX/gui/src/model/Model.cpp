#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "main.h"
#include "cmsis_os2.h"
#include "CR_CAN_parse.h"
#include "CR_structs.h"
#include "CR_encoder.h"
#include "CR_shift_light.h"
extern CR_CAN_vals latest_CAN_Vals;
extern CR_button_state menu_btn_state;
extern CR_button_state back_btn_state;
extern CR_encoder_status encoder_status;
extern CR_shift_light shift_light_handle;
extern CR_encoder encoder_UI_handle;
//extern osMessageQueueId_t ADCQueueHandle;
//extern osMessageQueueId_t CANMessageQueueHandle;
Model::Model() : modelListener(0)
{

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
	if(menu_btn_state == BUTTON_PRESSED){
		menu_btn_state = BUTTON_RELEASED;
		modelListener->toggle_menu();
		modelListener->cursor_down();
	}

	if(encoder_UI_handle.status == ENCODER_RIGHT){
		encoder_UI_handle.status = ENCODER_STANDBY;
		modelListener->cursor_down();
	} else if (encoder_UI_handle.status == ENCODER_LEFT){
		encoder_UI_handle.status = ENCODER_STANDBY;
		modelListener->cursor_up();
	}

}
