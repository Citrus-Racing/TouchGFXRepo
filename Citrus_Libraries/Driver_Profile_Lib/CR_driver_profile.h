#ifndef DRIVER_PROFILE_LIB_CR_DRIVER_PROFILE_H_
#define DRIVER_PROFILE_LIB_CR_DRIVER_PROFILE_H_

#include <stdint.h>
#include "CR_flash_storage.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint8_t r, g, b;      // fill color
    uint8_t br, bg, bb;   // border color
} CR_color_entry_t;

typedef struct {
    int16_t x, y, w, h;
} CR_box_geometry_t;

typedef struct {
    int16_t x, y, w;
} CR_profile_selector_pos_t;


#define CR_NUM_CUSTOM_BOXES   13
#define CR_BG_BOX_INDEX       7
#define CR_RESET_BOX_INDEX    12

extern const CR_color_entry_t   CR_COLOR_PALETTE[CR_NUM_PALETTE_COLORS];
extern const CR_box_geometry_t  CR_CUSTOMIZER_BOX_POS[CR_NUM_CUSTOM_BOXES];
extern const uint8_t            CR_BOX_SCROLL_ORDER[CR_NUM_CUSTOM_BOXES];
extern const int16_t            CR_PROFILE_ROW_Y[5];

// Profile selector layout constants
#define CR_PROFILE_STATUS_X    113
#define CR_PROFILE_EDIT_X      210
#define CR_PROFILE_STATUS_W    85
#define CR_PROFILE_EDIT_W      58
#define CR_PROFILE_SELECTOR_MAX 9

// Determine the status string for a profile slot.
// Returns "In Use", "Filled", or "Default".
const char* CR_profile_get_status_str(uint8_t index, uint8_t active_index,
                                      int filled, int has_default_colors);

// Check if a profile's colors are all default (all zeros).
int CR_profile_has_default_colors(const CR_profile_t* profile);

// Build a CR_profile_t from pending color arrays.
void CR_profile_build_from_pending(CR_profile_t* out,
                                   const uint8_t pending_colors[CR_NUM_BOX_COLORS],
                                   uint8_t pending_bg_color);

// Reset pending colors array to defaults (all 0 except fuel index 6 = 1).
void CR_profile_reset_pending(uint8_t pending_colors[CR_NUM_BOX_COLORS],
                              uint8_t* pending_bg_color);

// Wrapping increment within palette range [0, CR_NUM_PALETTE_COLORS).
uint8_t CR_color_cycle_next(uint8_t current);

// Wrapping decrement within palette range [0, CR_NUM_PALETTE_COLORS).
uint8_t CR_color_cycle_prev(uint8_t current);

// Compute the x, y, width for the profile selector given a selector index (0-9).
CR_profile_selector_pos_t CR_profile_get_selector_pos(uint8_t selector_index);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_PROFILE_LIB_CR_DRIVER_PROFILE_H_ */
