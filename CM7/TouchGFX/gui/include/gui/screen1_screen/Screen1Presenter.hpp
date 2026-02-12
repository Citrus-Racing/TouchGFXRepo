#ifndef SCREEN1PRESENTER_HPP
#define SCREEN1PRESENTER_HPP

#include "main.h"
#include <stdint.h>
#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
#include "CR_CAN_parse.h"


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
    void toggle_menu();
    void cursor_up();
    void cursor_down();

private:
    Screen1Presenter();

    Screen1View& view;
};

#endif // SCREEN1PRESENTER_HPP
