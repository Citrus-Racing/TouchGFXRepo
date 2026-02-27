#include "CR_driver_profile.h"
#include <string.h>


const CR_color_entry_t CR_COLOR_PALETTE[CR_NUM_PALETTE_COLORS] = {
    {  10,  10,  10,  255, 255, 255 },  // 0: Black
    { 255, 255, 255,  180, 180, 180 },  // 1: White   (triggers black border + black text)
    { 230,   0,   0,  161,   0,   0 },  // 2: Red
    {   0, 180,   0,    0, 126,   0 },  // 3: Green
    {   0,   0, 200,    0,   0, 140 },  // 4: Blue
    { 200, 200,   0,  140, 140,   0 },  // 5: Yellow
    { 220, 140,   0,  154,  98,   0 },  // 6: Orange
    { 140,   0, 200,   98,   0, 140 },  // 7: Purple
};

const CR_box_geometry_t CR_CUSTOMIZER_BOX_POS[CR_NUM_CUSTOM_BOXES] = {
    {  28,  59, 314, 106 },  // 0: bx_oilt_1
    {  28, 172, 314, 106 },  // 1: bx_oilp_1
    {  28, 280, 314, 106 },  // 2: bx_batt_1
    {  28, 387, 314, 106 },  // 3: bx_time_1
    { 355,  14, 314,  88 },  // 4: bx_rpm_1
    { 355, 101, 314, 278 },  // 5: bx_gear_1
    { 279, 508, 472,  68 },  // 6: dbx_fuel_1
    { 702,  -6, 266,  79 },  // 7: bx_change_bg
    { 682,  59, 314, 106 },  // 8: bx_speed_1
    { 682, 172, 314, 106 },  // 9: bx_DRS_1
    { 682, 280, 314, 106 },  // 10: bx_alag_1
    { 682, 387, 314, 106 },  // 11: bx_lch_1
    { 740, 503, 266,  79 },  // 12: bx_reset
};

const uint8_t CR_BOX_SCROLL_ORDER[CR_NUM_CUSTOM_BOXES] = {
    0, 1, 2, 3,    // left col:   oilt, oilp, batt, time
    4, 5, 6,       // center col: rpm, gear, fuel
    7,             // right col:  change_bg (cycles pending_bg_color)
    8, 9, 10, 11,  // right col:  speed, DRS, alag, lch
    12             // right col:  reset (triggers reset_customizer_to_defaults)
};

const int16_t CR_PROFILE_ROW_Y[5] = { 123, 157, 192, 228, 260 };

const char* CR_profile_get_status_str(uint8_t index, uint8_t active_index,
                                      int filled, int has_default_colors)
{
    if (filled && index == active_index)
        return "In Use";
    if (filled && !has_default_colors)
        return "Filled";
    return "Default";
}

int CR_profile_has_default_colors(const CR_profile_t* profile)
{
    for (int i = 0; i < CR_NUM_BOX_COLORS; i++) {
    	// Fuel Box Should be white
    	if(i==6 && profile->box_colors[i] != 1){
    		return 0;
    	// Everything else should be black
    	} else if (profile->box_colors[i] != 0) {
            return 0;
    	}
    }
    if (profile->bg_color != 0)
        return 0;
    return 1;
}

void CR_profile_build_from_pending(CR_profile_t* out,
                                   const uint8_t pending_colors[CR_NUM_BOX_COLORS],
                                   uint8_t pending_bg_color)
{
    out->magic = CR_PROFILE_MAGIC;
    for (int i = 0; i < CR_NUM_BOX_COLORS; i++)
        out->box_colors[i] = pending_colors[i];
    out->bg_color = pending_bg_color;
    out->reserved[0] = 0;
    out->reserved[1] = 0;
    out->reserved[2] = 0;
}

void CR_profile_reset_pending(uint8_t pending_colors[CR_NUM_BOX_COLORS],
                              uint8_t* pending_bg_color)
{
	// Sets all to black (fuel to white)
    for (int i = 0; i < CR_NUM_BOX_COLORS; i++)
        pending_colors[i] = (i == 6) ? 1 : 0;
    *pending_bg_color = 0;
}

uint8_t CR_color_cycle_next(uint8_t current)
{
    current++;
    if (current >= CR_NUM_PALETTE_COLORS)
        current = 0;
    return current;
}

uint8_t CR_color_cycle_prev(uint8_t current)
{
    if (current == 0)
        return CR_NUM_PALETTE_COLORS - 1;
    return current - 1;
}

CR_profile_selector_pos_t CR_profile_get_selector_pos(uint8_t selector_index)
{
    CR_profile_selector_pos_t pos;
    uint8_t row = selector_index / 2;
    int is_edit = (selector_index % 2) != 0;

    pos.x = is_edit ? CR_PROFILE_EDIT_X : CR_PROFILE_STATUS_X;
    pos.y = CR_PROFILE_ROW_Y[row];
    pos.w = is_edit ? CR_PROFILE_EDIT_W : CR_PROFILE_STATUS_W;
    return pos;
}
