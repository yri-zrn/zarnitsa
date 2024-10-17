#pragma once

#include "Layer.hpp"


#include <imgui.h>
#include <ImGuizmo.h>

// #ifdef IMGUI_INCLUDE_IMGUIZMO
// #error "Must include ImGuizmo"
// #endif

namespace zrn
{

namespace Internal
{
void _ImGuiBeginFrame();
void _ImGuiEndFrame();
void _ImGuiInit();
void _ImGuiTerminate();
} // namespace Internal

template <class T>
class ImGuiLayer : public Layer
{
public:
    using LayerType = T;

    ImGuiLayer();
};

template<class T>
inline ImGuiLayer<T>::ImGuiLayer()
{
    this->_Init = [](Layer* layer)->bool
    {
        if (ImGuiInstanceCount == 0)
            Internal::_ImGuiInit();
        ++ImGuiInstanceCount;
        return ((LayerType*)layer)->Init();
    };

    this->_Terminate = [](Layer* layer)
    {
        ((LayerType*)layer)->Terminate();
        if (ImGuiInstanceCount == 1)
            Internal::_ImGuiTerminate();
        --ImGuiInstanceCount;
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
    
    this->_OnImGuiRender = [](Layer* layer)
    {
        if (ImGuiRenderedInstanceCount == 0)
        {
            Internal::_ImGuiBeginFrame();
        }
        
        ((LayerType*)layer)->OnImGuiRender();
        ++ImGuiRenderedInstanceCount;

        if (ImGuiRenderedInstanceCount == ImGuiInstanceCount)
        {
            Internal::_ImGuiEndFrame();
            ImGuiRenderedInstanceCount = 0;
        }
    };
    
    this->_OnEnd = [](Layer* layer)
    {
        ((LayerType*)layer)->OnEnd();
    };
}

} // namespace zrn

namespace ImGui
{
    bool BeginPopupVoidTitle(const char* name, ImGuiPopupFlags popup_flags = 1);
} // namespace ImGui