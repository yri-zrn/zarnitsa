#pragma once

#include "Editor/PropertyEditor.hpp"

#include "ImGuiLayer/ImGuiLayer.hpp"
#include <flecs.h>

namespace zrn
{

class SandboxLayer;

class World;

class DebugLayer : public ImGuiLayer<DebugLayer>
{
public:
    bool Init();
    void Terminate();
    void OnBegin();
    void OnUpdate(Timestep step);
    void OnEvent(Event event);
    void OnImGuiRender();
    void OnEnd();

public:
    bool ShowDemoWindow = true;
    bool ShowAnotherWindow = false;
    ImVec4 ClearColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);

    void ShowFramerateOverlay(bool* show);

    bool InputCaptured();

private:
    SandboxLayer* m_SandboxLayer;
    World* m_World;

    bool m_ShowPropertyEditor = true;
    PropertyEditor m_PropertyEditor;
};

} // namespace zrn