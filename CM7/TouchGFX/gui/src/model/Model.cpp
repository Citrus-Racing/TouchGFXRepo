#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "main.h"
#include "cmsis_os2.h"
#include "CR_CAN_parse.h"
extern osMessageQueueId_t CAN_640_QueueHandle;
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
//	GPIO_PinState buttonState;
//	if (osMessageQueueGet(ButtonQueueHandle, &buttonState, NULL, 0) == osOK){
//		modelListener->updateGUIButton(buttonState);
//	}
//	uint16_t adc_val;
//	if (osMessageQueueGet(ADCQueueHandle, &adc_val, NULL, 0) == osOK){
//		modelListener->updatePotDial(adc_val);
//	}
	uint8_t CAN_read_buff[8];
	if (osMessageQueueGet(CAN_640_QueueHandle, CAN_read_buff, NULL, 0) == osOK) {

		//modelListener->updateTextbox((const char *) CAN_read_buff, 8);
	}

}
