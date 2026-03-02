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
    {  31,  14, 312, 106 },  // 0:  oilt
    {  31, 111, 312, 106 },  // 1:  oilp
    {  32, 207, 312, 106 },  // 2:  batt
    {  32, 303, 312, 106 },  // 3:  cooltemp
    {  31, 400, 312, 107 },  // 4:  fuelmix
    { 685, 400, 312, 106 },  // 5:  time
    { 356,  14, 314,  88 },  // 6:  rpm
    { 358, 101, 312, 278 },  // 7:  gear
    { 276, 502, 482,  79 },  // 8:  fuel bar
    { 685,  14, 312, 106 },  // 9:  speed
    { 685, 111, 312, 107 },  // 10: DRS
    { 685, 207, 312, 105 },  // 11: alag
    { 685, 303, 312, 106 },  // 12: lch
    {  19, 499, 267,  82 },  // 13: change_bg (bottom left)
    { 743, 502, 264,  80 },  // 14: reset
};

const uint8_t CR_BOX_SCROLL_ORDER[CR_NUM_CUSTOM_BOXES] = {
    0, 1, 2, 3, 4, 13,    // left col:    oilt, oilp, batt, cooltemp, fuelmix, change_bg
    6, 7, 8,           // center col:  rpm, gear, fuel
    9, 10, 11, 12, 5,  // right col:   speed, DRS, alag, lch, time
    14                  // bottom right: reset (triggers reset_customizer_to_defaults)
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
    	if(i==8 && profile->box_colors[i] != 1){
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
}

void CR_profile_reset_pending(uint8_t pending_colors[CR_NUM_BOX_COLORS],
                              uint8_t* pending_bg_color)
{
	// Sets all to black (fuel to white)
    for (int i = 0; i < CR_NUM_BOX_COLORS; i++)
        pending_colors[i] = (i == 8) ? 1 : 0;
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
