#pragma once

#include "Window.hpp"
#include "Layer.hpp"
#include "GraphicsContext.hpp"

#include "Common/Timestep.hpp"
#include "Common/Logger.hpp"
#include "Common/Assert.hpp"
#include "Common/Math.hpp"
#include "Common/Status.hpp"

#include "Scene/AssetManager.hpp"

#include <memory>
#include <functional>

namespace zrn
{

class DebugLayer;

class Application
{
public:
    Application() = default;

    bool Init();
    void Terminate();

    void Run();
    void Close();

    void OnBegin();
    void OnUpdate(Timestep step);
    void OnEvent(Event event);
    void OnImGuiRender();
    void OnEnd();

    void Attach(const std::string& layer_name, Layer& layer);

    Layer& GetLayer(const std::string& layer_name);

public:
    Platform        Platform;
    zrn::Window     Window;
    GraphicsContext Context;
    AssetManager    AssetManager;

private:
    bool m_CloseRequested = false;

    Time m_LastTime = 0.0f;

    Internal::LayerStack m_LayerStack;
};

inline void Application::Attach(const std::string& layer_name, Layer& layer)
{
    layer.Application = this;
    m_LayerStack.PushLayer(layer_name, layer);
}

inline Layer& Application::GetLayer(const std::string& layer_name)
{
    return m_LayerStack.GetLayer(layer_name);
}

} // namespace zrn