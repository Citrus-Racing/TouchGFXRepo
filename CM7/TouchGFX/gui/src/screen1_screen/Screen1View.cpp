#include <gui/screen1_screen/Screen1View.hpp>

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

void Screen1View::setButtonVisual(GPIO_PinState buttonState){
	if (buttonState == GPIO_PIN_SET){
		onButton.forceState(1);
	} else {
		onButton.forceState(0);
	}
	onButton.invalidate();
}
