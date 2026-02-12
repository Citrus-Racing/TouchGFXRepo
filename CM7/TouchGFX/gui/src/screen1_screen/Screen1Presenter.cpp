#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

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

void Screen1Presenter::toggle_menu(){
	view.toggle_menu();
}

