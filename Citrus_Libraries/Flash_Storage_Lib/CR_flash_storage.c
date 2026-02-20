#include "CR_flash_storage.h"

// Default values used when the flash sector has never been written or is corrupt
#define CR_DEFAULT_FUEL_LEVEL_TENTHS    55  // 5.5L - full tank (Guess - Change when known)

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

void CR_flash_write_settings(const CR_settings_t * settings)
{
    // HAL_FLASH_Program with FLASH_TYPEPROGRAM_FLASHWORD requires a 32-byte aligned buffer
    uint32_t flashword[8] = {0};

    // Pack the settings struct into the first bytes of the flashword
    flashword[0] = ((uint32_t)settings->fuel_level_tenths << 8) |
                   ((uint32_t)settings->magic);

    CR_flash_erase_settings();

    HAL_FLASH_Unlock();

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,
                      CR_SETTINGS_FLASH_ADDRESS,
                      (uint32_t)flashword);

    HAL_FLASH_Lock();
}

HAL_StatusTypeDef CR_flash_read_settings(CR_settings_t * settings)
{
    // Read the first 32-bit word from the flash address
    uint32_t raw = *((volatile uint32_t *)CR_SETTINGS_FLASH_ADDRESS);

    uint8_t magic            = (uint8_t)(raw & 0xFF);
    uint8_t fuel_level_tenths = (uint8_t)((raw >> 8) & 0xFF);

    if(magic != CR_SETTINGS_MAGIC || fuel_level_tenths > 55)
    {
        // Sector uninitialised or data corrupt - use default
        settings->magic             = CR_SETTINGS_MAGIC;
        settings->fuel_level_tenths = CR_DEFAULT_FUEL_LEVEL_TENTHS;
        return HAL_ERROR;
    }

    settings->magic             = magic;
    settings->fuel_level_tenths = fuel_level_tenths;
    return HAL_OK;
}
