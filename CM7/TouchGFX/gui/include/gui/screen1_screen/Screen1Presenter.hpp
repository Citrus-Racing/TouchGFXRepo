#ifndef SCREEN1PRESENTER_HPP
#define SCREEN1PRESENTER_HPP

#include "main.h"
#include <stdint.h>
#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
#include "CR_CAN_parse.h"
#include "CR_flash_storage.h"


using namespace touchgfx;

class Screen1View;

class Screen1Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen1Presenter(Screen1View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Screen1Presenter() {}

    void update_CAN_info(CR_CAN_vals * CAN_data);
    void open_menu();
    void close_menu();
    void cursor_up();
    void cursor_down();
    void encoder_click();

    // Fuel level accessors called by the view
    uint8_t getFuelLevel();
    void saveFuelLevel(uint8_t tenths);

    // Driver profile accessors called by the view
    bool isProfileFilled(uint8_t index);
    void getProfile(uint8_t index, CR_profile_t * out);
    void saveProfile(uint8_t index, const CR_profile_t * profile);
    void setActiveProfile(uint8_t index);
    uint8_t getActiveProfileIndex();
    void saveAll();

private:
    Screen1Presenter();

    Screen1View& view;
};

#endif // SCREEN1PRESENTER_HPP
