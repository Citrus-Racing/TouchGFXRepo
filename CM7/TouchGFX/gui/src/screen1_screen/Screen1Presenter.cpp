#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "CR_flash_storage.h"

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

//void Screen1Presenter::updateGUIButton(GPIO_PinState status){
//	view.setButtonVisual(status);
//}
//
//void Screen1Presenter::updatePotDial(uint16_t adc_val){
//	view.setPotDialVal(adc_val);
//}
//
//void Screen1Presenter::updateTextbox(const char * message, uint8_t bytes){
//	view.setTextBox(message, bytes);
//}

void Screen1Presenter::update_CAN_info(CR_CAN_vals * CAN_data){
	view.update_CAN_info(CAN_data);
}

void Screen1Presenter::open_menu(){
	view.open_menu();
}

void Screen1Presenter::close_menu(){
	view.close_menu();
}

void Screen1Presenter::cursor_up(){
	view.cursor_up();
}

void Screen1Presenter::cursor_down(){
	view.cursor_down();
}

void Screen1Presenter::encoder_click(){
	view.encoder_click();
}

uint8_t Screen1Presenter::getFuelLevel(){
	return model->fuel_level_tenths;
}

void Screen1Presenter::saveFuelLevel(uint8_t tenths){
    model->fuel_level_tenths = tenths;
    model->saveAll();
}

bool Screen1Presenter::isProfileFilled(uint8_t index){
    return (index < CR_MAX_PROFILES && model->profiles[index].magic == CR_PROFILE_MAGIC);
}

void Screen1Presenter::getProfile(uint8_t index, CR_profile_t * out){
    if(index < CR_MAX_PROFILES) *out = model->profiles[index];
}

void Screen1Presenter::saveProfile(uint8_t index, const CR_profile_t * profile){
    if(index < CR_MAX_PROFILES) model->profiles[index] = *profile;
}

void Screen1Presenter::setActiveProfile(uint8_t index){
    model->active_profile_index = index;
}

uint8_t Screen1Presenter::getActiveProfileIndex(){
    return model->active_profile_index;
}

void Screen1Presenter::saveAll(){
    model->saveAll();
}
