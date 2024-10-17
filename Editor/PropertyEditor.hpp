#pragma once

#include <imgui.h>
#include <flecs.h>

namespace zrn
{

class SandboxLayer;

struct PropertyEditor
{
public:
    void Init(SandboxLayer* layer);

    ImGuiTextFilter Filter;

    void Show(bool* show);

private:
    void DrawNode(flecs::entity entity);
    void DrawComponent(flecs::entity entity, flecs::id component_id);

    void ShowEntityContextMenu();

    SandboxLayer* m_Layer;
};

} // namespace zrn