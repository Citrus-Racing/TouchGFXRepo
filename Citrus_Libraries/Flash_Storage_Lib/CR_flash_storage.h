#ifndef CR_FLASH_STORAGE_H_
#define CR_FLASH_STORAGE_H_

#include "stm32h7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Flash sector reserved for dashboard settings. (Fuel level and the active profile)
// Sector 4 of Bank 1 starts at 0x08080000. Chosen from an educated guess on where it should be written
// Nothing has broken yet
#define CR_SETTINGS_FLASH_ADDRESS   0x08080000
#define CR_SETTINGS_FLASH_SECTOR    FLASH_SECTOR_4
#define CR_SETTINGS_FLASH_BANK      FLASH_BANK_1

// Profiles start one flashword (32 bytes) after settings
#define CR_PROFILES_FLASH_BASE      0x08080020
#define CR_PROFILE_FLASHWORD_SIZE   32

// Magic byte written alongside data so we can detect if uninitialised sector
// and then fall back to a default rather than reading garbage. (Claude thinks this is smart)
#define CR_SETTINGS_MAGIC           0xA5
#define CR_PROFILE_MAGIC            0xB7

#define CR_MAX_PROFILES             5
#define CR_NUM_BOX_COLORS           13
#define CR_NUM_PALETTE_COLORS       8

// Layout of the settings flashword (32 bytes = 256-bit HAL flashword).
// Byte 0: magic byte
// Byte 1: fuel_level_tenths (0-55)
// Byte 2: active_profile_index (0-4, or 0xFF = no active profile)
typedef struct
{
    uint8_t magic;
    uint8_t fuel_level_tenths;
    uint8_t active_profile_index;
} CR_settings_t;

// A single driver profile stored in one flashword.
// Byte 0: magic (0xB7 if filled, 0xFF if empty/erased)
// Bytes 1-13: color palette index (0-7) for each dashboard box
// Byte 14: background color palette index
// Byte 15: reserved padding
typedef struct
{
    uint8_t magic;
    uint8_t box_colors[CR_NUM_BOX_COLORS];
    uint8_t bg_color;
    uint8_t reserved[1];
} CR_profile_t;

// Erase the settings sector in preparation for a write.
void CR_flash_erase_settings(void);

// Write all persistent data (settings + 5 profiles) to flash.
// Erases the sector once, then writes 6 flashwords.
void CR_flash_write_all(const CR_settings_t * settings, const CR_profile_t profiles[CR_MAX_PROFILES]);

// Read settings from flash into the provided struct.
// If the magic byte does not match (uninitialised or corrupt sector),
// the struct is populated with default values and HAL_ERROR is returned.
HAL_StatusTypeDef CR_flash_read_settings(CR_settings_t * settings);

// Read a single profile from flash. Returns HAL_ERROR if magic does not match.
HAL_StatusTypeDef CR_flash_read_profile(uint8_t index, CR_profile_t * profile);

// Read all 5 profiles from flash. Empty slots will have magic = 0xFF.
void CR_flash_read_all_profiles(CR_profile_t profiles[CR_MAX_PROFILES]);

#ifdef __cplusplus
}
#endif

#endif /* CR_FLASH_STORAGE_H_ */
