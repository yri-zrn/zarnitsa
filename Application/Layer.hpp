#pragma once

#include "Common/Timestep.hpp"
#include "Platform/Events.hpp"

#include <map>
#include <string>

namespace zrn
{

#define _BIND_LAYER_FUNC(func_name, func_block) \
    { this->_##func_name = [](Layer* layer) { func_block }; }\

#define BIND_CUSTOM_LAYER_FUNC(func_name, func) \
    _BIND_LAYER_FUNC( func_name, ((LayerType*)layer)->func(); )

#define BIND_LAYER_FUNC(func_name) BIND_CUSTOM_LAYER_FUNC(func_name, func_name)

class Application;

class Layer
{
public:
    static Application* Application;
    static size_t ImGuiInstanceCount;
    static size_t ImGuiRenderedInstanceCount;

    bool (*_Init)(Layer*)                = [](Layer*)->bool{ return true; };
    void (*_Terminate)(Layer*)           = [](Layer*){ };
    void (*_OnBegin)(Layer*)             = [](Layer*){ };
    void (*_OnUpdate)(Layer*, Timestep)  = [](Layer*, Timestep){ };
    void (*_OnEvent)(Layer*, Event)      = [](Layer*, Event){ };
    void (*_OnEnd)(Layer*)               = [](Layer*){ };
    void (*_OnImGuiRender)(Layer*)       = [](Layer*){ };
};

inline Application* Layer::Application = nullptr;
inline size_t Layer::ImGuiInstanceCount = 0;
inline size_t Layer::ImGuiRenderedInstanceCount = 0;

namespace Internal
{

class LayerStack
{
public:
    LayerStack() { }

    void PushLayer(const std::string& layer_name, Layer& layer);

    size_t Size() { return m_Stack.size(); }
    Layer& GetLayer(const std::string& name);

    auto begin()  { return m_Stack.begin();  }
    auto end()    { return m_Stack.end();    }

    auto rbegin() { return m_Stack.rbegin(); }
    auto rend()   { return m_Stack.rend();   }

private:
    std::vector<Layer*> m_Stack;
    std::map<std::string, Layer*> m_LayerNames;
};

inline void LayerStack::PushLayer(const std::string& layer_name, Layer& layer)
{
    m_LayerNames[layer_name] = &layer;
    m_Stack.push_back(&layer);
}

inline Layer& LayerStack::GetLayer(const std::string& layer_name)
{
    return *m_LayerNames.at(layer_name);
}

} // namespace Internal

} // namespace zrn