#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include "main.h"
#include <gui/model/Model.hpp>
#include <stdio.h>
#include "CR_CAN_parse.h"

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void update_CAN_info(CR_CAN_vals * CAN_data){}
    virtual void open_menu(){}
    virtual void close_menu(){}
    virtual void cursor_up(){}
    virtual void cursor_down(){}
    virtual void encoder_click(){}

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
