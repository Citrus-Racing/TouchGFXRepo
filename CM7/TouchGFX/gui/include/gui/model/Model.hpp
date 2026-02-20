#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

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

protected:
    ModelListener* modelListener;

private:
    // Ticks where button input is ignored
    // Fixes issue where clicking the button once
    // caused multiple menu clicks
    uint8_t menu_btn_debounce_ticks;
    uint8_t back_btn_debounce_ticks;

    static const uint8_t BTN_DEBOUNCE_TICKS = 30;
};

#endif // MODEL_HPP
