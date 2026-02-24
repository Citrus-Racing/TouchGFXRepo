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

    // Driver profiles selector: 10 positions (5 profiles x 2 columns)
    uint8_t profile_selector_index = 0;
    static const uint8_t  PROFILE_SELECTOR_MAX = 9;
    static const int16_t  PROFILE_STATUS_X = 113;
    static const int16_t  PROFILE_EDIT_X   = 210;
    static const int16_t  PROFILE_STATUS_W = 85; // Box needs to change width based on if its on "Status" or "Edit"
    static const int16_t  PROFILE_EDIT_W   = 58;
    static constexpr int16_t PROFILE_ROW_Y[5] = { 123, 157, 192, 228, 260 };

    void update_profile_selector();
    void refresh_profile_status_texts();

    // Color palette
    struct ColorEntry {
        uint8_t r, g, b;      // fill color
        uint8_t br, bg, bb;   // border color
    };
    static const ColorEntry COLOR_PALETTE[CR_NUM_PALETTE_COLORS];

    // Display customizer state
    // 13 scrollable positions (ordered left col, center col, right col):
    //  0:oilt 1:oilp 2:batt 3:time 4:rpm 5:gear 6:fuel
    //  7:change_bg 8:speed 9:DRS 10:alag 11:lch 12:reset

    static const uint8_t NUM_CUSTOM_BOXES = 13;   // total scrollable positions
    static const uint8_t BG_BOX_INDEX = 7;        // scroll pos for bx_change_bg
    static const uint8_t RESET_BOX_INDEX = 12;    // scroll pos for bx_reset (click-only, no color)

    uint8_t customizer_scroll_pos = 0;      // current position in scroll order (0-12)
    bool    color_edit_mode = false;        // true = encoder cycles palette colors
    uint8_t editing_profile_index = 0;      // which profile slot we're editing (0-4)
    uint8_t pending_colors[CR_NUM_BOX_COLORS] = {};  // working copy of 11 box color indices
    uint8_t pending_bg_color = 0;           // working copy of dashboard background color index

    // Scroll order maps encoder positions to box indices (identity mapping for this layout)
    static constexpr uint8_t BOX_SCROLL_ORDER[NUM_CUSTOM_BOXES] = {
        0, 1, 2, 3,    // left col:   oilt, oilp, batt, time
        4, 5, 6,        // center col: rpm, gear, fuel
        7,              // right col:  change_bg (cycles pending_bg_color)
        8, 9, 10, 11,  // right col:  speed, DRS, alag, lch
        12              // right col:  reset (triggers reset_customizer_to_defaults)
    };

    // Bounding rectangles for the widget selector highlight around each customizer box
    struct BoxGeometry { int16_t x, y, w, h; };
    static const BoxGeometry CUSTOMIZER_BOX_POS[NUM_CUSTOM_BOXES];

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

    // Reset all pending colors and bg to default (black, palette index 0)
    // and refresh the customizer preview. Does not save to flash.
    void reset_customizer_to_defaults();
};

#endif // SCREEN1VIEW_HPP
