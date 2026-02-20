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
    void encoder_click();

protected:

private:
    bool fuel_menu_open;

    // Staged fuel level while the user is scrolling, stored in tenths of a
    // litre (0-55 represents 0.0L - 5.5L). Committed to model on save.
    uint8_t pending_fuel_tenths;

    // Width of dbx_fuel at 100% (5.5L). Anchored left at x=294.
    static const uint16_t FUEL_BAR_MAX_WIDTH  = 442;
    static const uint8_t  FUEL_MAX_TENTHS     = 55;   // 5.5L in tenths

    // Update dtxt_set_level text and dbx_fuel bar for a given tenths value
    void refresh_fuel_display(uint8_t tenths);
};

#endif // SCREEN1VIEW_HPP
