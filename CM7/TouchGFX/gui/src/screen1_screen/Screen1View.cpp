#include <gui/screen1_screen/Screen1View.hpp>


// Hi there!
// Remember to specify which letters you want loaded in TouchGFX.
/// I disabled the Fallback "?" unknown character, so you'll see nothing if you don't.


Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void set_dtxt_rpm(uint16_t engine_speed){
    //Unicode::itoa(engine_speed,dtxt_rpmBuffer,11,10);
}

//void Screen1View::setButtonVisual(GPIO_PinState buttonState){
//	if (buttonState == GPIO_PIN_SET){
//		onButton.forceState(1);
//	} else {
//		onButton.forceState(0);
//	}
//	onButton.invalidate();
//}
//
//void Screen1View::setPotDialVal(uint16_t analogVal){
//	int normalizedVal = (int) ((analogVal/65536.0)*100);
//    VirtSpedometer.setValue(normalizedVal);
//    VirtSpedometer.invalidate();
//}
//
//void Screen1View::setTextBox(const char * message, uint8_t bytes){
//	Unicode::strncpy(canMessageBoxBuffer, message, bytes);
//	canMessageBox.invalidate();
//}
