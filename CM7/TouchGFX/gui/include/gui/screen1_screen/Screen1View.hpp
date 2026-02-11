#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include "main.h"
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

//    void setButtonVisual(GPIO_PinState buttonState);
//    void setPotDialVal(uint16_t analogVal);
//    void setTextBox(const char * message, uint8_t bytes);
    void set_dtxt_rpm(uint16_t engine_speed);
protected:
};

#endif // SCREEN1VIEW_HPP
