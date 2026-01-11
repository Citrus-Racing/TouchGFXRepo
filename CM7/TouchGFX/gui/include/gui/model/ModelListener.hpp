#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include "main.h"
#include <gui/model/Model.hpp>
#include <stdio.h>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void updateGUIButton(GPIO_PinState status) {}
    virtual void updatePotDial(uint16_t adc_val) {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
