#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include "main.h"
#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void updateGUIButton(GPIO_PinState status) {} // status = 1 or 0

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
