#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include "main.h"
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "CR_CAN_parse.h"
#include "CR_flash_storage.h"

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
    bool dashboard_open = true;

    // Staged fuel level while the user is scrolling, stored in tenths of a
    // litre (0-55 represents 0.0L - 5.5L). Committed to model on save.
    uint8_t pending_fuel_tenths = 55;

    // Width of dbx_fuel at 100% (5.5L). Anchored left at x=294.
    static const uint16_t FUEL_BAR_MAX_WIDTH  = 442;
    static const uint8_t  FUEL_MAX_TENTHS     = 55;   // 5.5L in tenths

    void refresh_fuel_display(uint8_t tenths);

    // --- Driver profiles selector: 10 positions (5 profiles x 2 columns) ---
    uint8_t profile_selector_index = 0;
    static const uint8_t  PROFILE_SELECTOR_MAX = 9;
    static const int16_t  PROFILE_STATUS_X = 113;
    static const int16_t  PROFILE_EDIT_X   = 210;
    static const int16_t  PROFILE_STATUS_W = 74;
    static const int16_t  PROFILE_EDIT_W   = 58;
    static constexpr int16_t PROFILE_ROW_Y[5] = { 123, 157, 192, 228, 260 };

    void update_profile_selector();
    void refresh_profile_status_texts();

    // --- Color palette ---
    struct ColorEntry {
        uint8_t r, g, b;      // fill color
        uint8_t br, bg, bb;   // border color
    };
    static const ColorEntry COLOR_PALETTE[CR_NUM_PALETTE_COLORS];

    // --- Display customizer state ---
    // 11 customizable boxes:
    //  0:speed 1:rpm 2:gear 3:fuel(dbx_fuel) 4:oilt 5:oilp 6:batt 7:time 8:DRS 9:alag 10:lch
    static const uint8_t NUM_CUSTOM_BOXES = 11;

    uint8_t customizer_scroll_pos = 0;      // position in scroll order (0-10)
    bool    color_edit_mode = false;        // true = encoder cycles colors
    uint8_t editing_profile_index = 0;      // which profile slot (0-4)
    uint8_t pending_colors[NUM_CUSTOM_BOXES] = {};  // working copy of color indices

    // Scroll order: left column top-down, center top-down, right column top-down
    // Maps scroll position → box index in CUSTOMIZER_BOX_POS / pending_colors
    static constexpr uint8_t BOX_SCROLL_ORDER[NUM_CUSTOM_BOXES] = {
        0, 5, 6, 7,    // left:   speed, oilp, batt, time
        1, 2, 3,        // center: rpm, gear, fuel(dbx_fuel)
        4, 8, 9, 10    // right:  oilt, DRS, alag, lch
    };

    // Box positions in the customizer for the widget selector highlight
    struct BoxGeometry { int16_t x, y, w, h; };
    static const BoxGeometry CUSTOMIZER_BOX_POS[NUM_CUSTOM_BOXES];

    void apply_colors_to_dashboard(const uint8_t colors[NUM_CUSTOM_BOXES]);
    void apply_colors_to_customizer(const uint8_t colors[NUM_CUSTOM_BOXES]);
    void apply_single_box_color_customizer(uint8_t box_index, uint8_t color_index);
    void update_widget_selector();
    void open_customizer_for_profile(uint8_t profile_index);
    void save_and_apply_customizer();
};

#endif // SCREEN1VIEW_HPP
