#include "CR_flash_storage.h"
#include <string.h>

// Default values used when the flash sector has never been written or is corrupt
#define CR_DEFAULT_FUEL_LEVEL_TENTHS    55   // 5.5L - full tank (Guess - Change when known)
#define CR_DEFAULT_ACTIVE_PROFILE       0xFF // no active profile

void CR_flash_erase_settings(void)
{
    FLASH_EraseInitTypeDef erase_def;
    uint32_t erase_fault = 0;

    HAL_FLASH_Unlock();

    erase_def.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase_def.Banks        = CR_SETTINGS_FLASH_BANK;
    erase_def.NbSectors    = 1;
    erase_def.Sector       = CR_SETTINGS_FLASH_SECTOR;
    erase_def.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASHEx_Erase(&erase_def, &erase_fault);

    HAL_FLASH_Lock();
}

void CR_flash_write_all(const CR_settings_t * settings, const CR_profile_t profiles[CR_MAX_PROFILES])
{
    uint32_t flashword[8];

    CR_flash_erase_settings();

    HAL_FLASH_Unlock();

    // Write settings flashword at 0x08080000
    memset(flashword, 0, sizeof(flashword));
    flashword[0] = ((uint32_t)settings->active_profile_index << 16) |
                   ((uint32_t)settings->fuel_level_tenths << 8) |
                   ((uint32_t)settings->magic);

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,
                      CR_SETTINGS_FLASH_ADDRESS,
                      (uint32_t)flashword);

    // Write each profile flashword
    for(uint8_t i = 0; i < CR_MAX_PROFILES; i++)
    {
        memset(flashword, 0, sizeof(flashword));

        uint8_t * bytes = (uint8_t *)flashword;
        bytes[0] = profiles[i].magic;
        for(int c = 0; c < CR_NUM_BOX_COLORS; c++){
            bytes[1 + c] = profiles[i].box_colors[c];
        }
        bytes[12] = profiles[i].bg_color; // dashboard background color index

        uint32_t addr = CR_PROFILES_FLASH_BASE + (i * CR_PROFILE_FLASHWORD_SIZE);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr, (uint32_t)flashword);
    }

    HAL_FLASH_Lock();
}

HAL_StatusTypeDef CR_flash_read_settings(CR_settings_t * settings)
{
    // Read the first 32-bit word from the flash address
    uint32_t raw = *((volatile uint32_t *)CR_SETTINGS_FLASH_ADDRESS);

    uint8_t magic             = (uint8_t)(raw & 0xFF);
    uint8_t fuel_level_tenths = (uint8_t)((raw >> 8) & 0xFF);
    uint8_t active_profile    = (uint8_t)((raw >> 16) & 0xFF);

    if(magic != CR_SETTINGS_MAGIC || fuel_level_tenths > 55)
    {
        // Sector uninitialised or data corrupt - use defaults
        settings->magic                = CR_SETTINGS_MAGIC;
        settings->fuel_level_tenths    = CR_DEFAULT_FUEL_LEVEL_TENTHS;
        settings->active_profile_index = CR_DEFAULT_ACTIVE_PROFILE;
        return HAL_ERROR;
    }

    settings->magic                = magic;
    settings->fuel_level_tenths    = fuel_level_tenths;
    settings->active_profile_index = active_profile;
    return HAL_OK;
}

HAL_StatusTypeDef CR_flash_read_profile(uint8_t index, CR_profile_t * profile)
{
    if(index >= CR_MAX_PROFILES) return HAL_ERROR;

    uint32_t addr = CR_PROFILES_FLASH_BASE + (index * CR_PROFILE_FLASHWORD_SIZE);
    volatile uint8_t * raw = (volatile uint8_t *)addr;

    profile->magic = raw[0];
    for(int c = 0; c < CR_NUM_BOX_COLORS; c++){
        profile->box_colors[c] = raw[1 + c];
    }
    profile->bg_color = raw[12]; // dashboard background color index
    profile->reserved[0] = 0;
    profile->reserved[1] = 0;
    profile->reserved[2] = 0;

    if(profile->magic != CR_PROFILE_MAGIC) return HAL_ERROR;
    return HAL_OK;
}

void CR_flash_read_all_profiles(CR_profile_t profiles[CR_MAX_PROFILES])
{
    for(uint8_t i = 0; i < CR_MAX_PROFILES; i++){
        if(CR_flash_read_profile(i, &profiles[i]) != HAL_OK){
            profiles[i].magic = 0xFF;  // mark as empty
        }
    }
}
