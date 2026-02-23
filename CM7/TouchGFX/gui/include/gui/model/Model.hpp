#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>
#include "CR_flash_storage.h"

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    // Fuel level in tenths of a litre (0-55 represents 0.0L - 5.5L)
    uint8_t fuel_level_tenths;

    // Driver profiles
    uint8_t active_profile_index;
    CR_profile_t profiles[CR_MAX_PROFILES];

    // Persist all settings + profiles to flash
    void saveAll();

protected:
    ModelListener* modelListener;

private:
    // Ticks where input is ignored after triggering
    // Fixes issue where a single click/turn registers multiple times
    uint8_t menu_btn_debounce_ticks;
    uint8_t back_btn_debounce_ticks;
    uint8_t encoder_debounce_ticks;

    static const uint8_t BTN_DEBOUNCE_TICKS = 30;
    static const uint8_t ENCODER_DEBOUNCE_TICKS = 30;
};

#endif // MODEL_HPP
