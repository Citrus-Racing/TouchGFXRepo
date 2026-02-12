#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include "main.h"
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "CR_CAN_parse.h"

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
    void update_CAN_info(CR_CAN_vals * CAN_data);
    void toggle_menu();
    void open_menu();
    void close_menu();
    void cursor_up();
    void cursor_down();

protected:
};

#endif // SCREEN1VIEW_HPP
