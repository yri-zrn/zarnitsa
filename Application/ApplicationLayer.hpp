#pragma once

#include "Layer.hpp"

namespace zrn
{

template <class T>
class ApplicationLayer : public Layer
{
public:
    using LayerType = T;

    ApplicationLayer()
    {
        this->_Init = [](Layer* layer)->bool
        {
            return ((LayerType*)layer)->Init();
        };

        this->_Terminate = [](Layer* layer)
        {
            ((LayerType*)layer)->Terminate();
        };

        this->_OnBegin = [](Layer* layer)
        {
            ((LayerType*)layer)->OnBegin();
        };

        this->_OnUpdate = [](Layer* layer, Timestep timestep)
        {
            ((LayerType*)layer)->OnUpdate(timestep);
        };

        this->_OnEvent = [](Layer* layer, Event event)
        {
            ((LayerType*)layer)->OnEvent(event);
        };

        this->_OnEnd = [](Layer* layer)
        {
            ((LayerType*)layer)->OnEnd();
        };
    }
};

} // namespace zrn
