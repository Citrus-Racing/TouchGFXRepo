#ifndef CR_FLASH_STORAGE_H_
#define CR_FLASH_STORAGE_H_

#include "stm32h7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Flash sector reserved for dashboard settings.
// Sector 4 of Bank 1 starts at 0x08080000. Chosen from an educated guess on where it should be written
// Nothing has broken yet
#define CR_SETTINGS_FLASH_ADDRESS   0x08080000
#define CR_SETTINGS_FLASH_SECTOR    FLASH_SECTOR_4
#define CR_SETTINGS_FLASH_BANK      FLASH_BANK_1

// Magic byte written alongside data so we can detect if uninitialised sector
// and then fall back to a default rather than reading garbage. (Claude thinks this is smart)
#define CR_SETTINGS_MAGIC           0xA5

// Layout of the settings flashword (32 bytes = 256-bit HAL flashword).
// Only using 2 bytes for the fuel
// Byte 0: magic byte
// Byte 1: fuel_level_tenths (0-55)
typedef struct
{
    uint8_t magic;
    uint8_t fuel_level_tenths;
} CR_settings_t;

// Erase the settings sector in preparation for a write.
// Must be called before CR_flash_write_settings.
void CR_flash_erase_settings(void);

// Write settings to flash. Erases the sector first internally.
void CR_flash_write_settings(const CR_settings_t * settings);

// Read settings from flash into the provided struct.
// If the magic byte does not match (uninitialised or corrupt sector),
// the struct is populated with default value (full tank) and HAL_ERROR is returned.
HAL_StatusTypeDef CR_flash_read_settings(CR_settings_t * settings);

#ifdef __cplusplus
}
#endif

#endif /* CR_FLASH_STORAGE_H_ */
