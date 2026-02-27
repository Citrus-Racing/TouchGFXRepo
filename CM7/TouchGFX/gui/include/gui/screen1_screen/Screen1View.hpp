#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include "main.h"
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "CR_CAN_parse.h"
#include "CR_flash_storage.h"
#include "CR_driver_profile.h"

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void update_CAN_info(CR_CAN_vals * CAN_data);
    void toggle_menu();
    void open_menu();
    void close_menu();
    void cursor_up();
    void cursor_down();
    void encoder_click();

protected:

private:
    bool fuel_menu_open = false;
    bool main_menu_open = false;
    bool driver_profiles_menu_open = false;
    bool customize_profile_open = false;
    bool sensor_readout_open = false;
    bool aim_readout_open = false;
    bool error_status_open = false;
    bool dashboard_open = true;

    // Staged fuel level while the user is scrolling, stored in tenths of a
    // litre (0-55 represents 0.0L - 5.5L). Committed to model on save.
    uint8_t pending_fuel_tenths = 55;

    // Width of dbx_fuel at 100% (5.5L). Anchored left at x=294.
    static const uint16_t FUEL_BAR_MAX_WIDTH  = 442;
    static const uint8_t  FUEL_MAX_TENTHS     = 55;   // 5.5L in tenths

    void refresh_fuel_display(uint8_t tenths);

    // Driver profiles selector: 10 positions (5 profiles x 2 columns)
    uint8_t profile_selector_index = 0;

    void update_profile_selector();
    void refresh_profile_status_texts();

    // Display customizer state
    uint8_t customizer_scroll_pos = 0;      // current position in scroll order (0-12)
    bool    color_edit_mode = false;        // true = encoder cycles palette colors
    uint8_t editing_profile_index = 0;      // which profile slot we're editing (0-4)
    uint8_t pending_colors[CR_NUM_BOX_COLORS] = {};  // working copy of 11 box color indices
    uint8_t pending_bg_color = 0;           // working copy of dashboard background color index

    // Apply palette colors to all dashboard boxes, text, borders, and background.
    void apply_colors_to_dashboard(const uint8_t colors[CR_NUM_BOX_COLORS], uint8_t bg_color);

    // Same as above but for the customizer preview (the _1 widgets).
    // Also updates bx_change_bg, txt_change_background, and txt_title.
    void apply_colors_to_customizer(const uint8_t colors[CR_NUM_BOX_COLORS], uint8_t bg_color);

    // Update a single customizer box during live color editing (called by cursor_up/down).
    // Handles fill, border, and text color for the given scroll-order box_index.
    void apply_single_box_color_customizer(uint8_t box_index, uint8_t color_index);

    void update_widget_selector();
    void open_customizer_for_profile(uint8_t profile_index);
    void save_and_apply_customizer();

    // Reset all pending colors and bg to default
    // and refresh the customizer preview. Does not save to flash.
    void reset_customizer_to_defaults();
};

#endif // SCREEN1VIEW_HPP
