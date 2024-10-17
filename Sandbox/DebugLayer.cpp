#include "DebugLayer.hpp"

#include "Application.hpp"
#include "Sandbox/SandboxLayer.hpp"
#include "Scene/Components.hpp"

#include "Application/ImGuiLayer/imgui_impl_glfw.h"
#include "Application/ImGuiLayer/imgui_impl_opengl3.h"
#include "Application/ImGuiLayer/imgui_impl_opengl3_loader.h"

#include <GLFW/glfw3.h>

#include "Common/Math.hpp"

namespace zrn
{

bool DebugLayer::Init()
{
    m_SandboxLayer = (zrn::SandboxLayer*)&Application->GetLayer("SandboxLayer");
    m_World = &m_SandboxLayer->World;

    m_PropertyEditor.Init(m_SandboxLayer);

    return true;
}

void DebugLayer::Terminate()
{

}

void DebugLayer::OnBegin()
{

}

void DebugLayer::OnUpdate(Timestep timestep)
{
    (void)timestep;
}

void DebugLayer::OnEvent(Event event)
{
    (void)event;
}

void DebugLayer::OnImGuiRender()
{
    // auto[width, height] = Application->Window().GetFramebufferSize();
    ImGuiIO& io = ImGui::GetIO();
    float width = (float)ImGui::GetWindowWidth(); (void)width;
    float height = (float)ImGui::GetWindowHeight(); (void)height;

    static bool show_framerate = true;
    ShowFramerateOverlay(&show_framerate);
    
    if (ShowDemoWindow)
        ImGui::ShowDemoWindow(&ShowDemoWindow);

    //! TEMP: ImGuizmo stuff
    #define IMGUIZMO
    #ifdef IMGUIZMO
    {
        static ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE gizmo_mode = ImGuizmo::LOCAL;

        // ImGui::Begin("Viewport");

        if (m_World->SelectedEntity && gizmo_operation != -1)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_1))
            {
                gizmo_mode = ImGuizmo::LOCAL;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_2))
            {
                gizmo_mode = ImGuizmo::WORLD;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_G))
            {
                gizmo_operation = ImGuizmo::TRANSLATE;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_R))
            {
                gizmo_operation = ImGuizmo::ROTATE;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_S))
            {
                gizmo_operation = ImGuizmo::SCALE;
            }

            ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));

            ImGuizmo::SetOrthographic(false);
            // ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            ImGuizmo::BeginFrame();

            // Camera
            const glm::mat4& camera_view = m_World->Camera.View;
            const glm::mat4& camera_projection = m_World->Camera.Projection;

            bool snap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
            float snap_value = 0.5f;
            if (gizmo_operation == ImGuizmo::OPERATION::ROTATE)
                snap_value = 15.0f;
            float snap_values[3] = { snap_value, snap_value, snap_value };

            // Entity transform
            auto entity = m_World->SelectedEntity;
            auto* transform_comp = entity.get_mut<TransformComponent, GlobalTag>();
            auto* transform_comp_local = entity.get_mut<TransformComponent, LocalTag>();
            auto global_transform = transform_comp->Tranfsorm();

            ImGuizmo::Manipulate(
                glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                gizmo_operation,
                gizmo_mode,
                glm::value_ptr(global_transform),
                nullptr, snap ? snap_values : nullptr
            );

            if (ImGuizmo::IsUsing())
            {
                // global_transform = parent_transform * local_child_transform
                // local_child_transform = (parent_transform ^ -1) * global_transform

                auto local_transform = global_transform;
                if (auto parent = entity.parent())
                {
                    auto parent_transform = parent.get<TransformComponent, GlobalTag>()->Tranfsorm();
                    local_transform = glm::inverse(parent_transform) * local_transform;
                }

                glm::vec3 translation, rotation, scale;
                glm::quat orientation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(local_transform, scale, orientation, translation, skew, perspective);
                rotation = glm::eulerAngles(orientation);

                transform_comp_local->Translation = translation;
                transform_comp_local->Rotation    = rotation;
                transform_comp_local->Scale       = scale;
                // transform_comp_local->Skew        = skew;
            }

            ImGui::PopStyleColor();
        }
        // ImGui::End();
    }
    #endif

    ImGui::Begin("Framebuffer test");
    {
        ImGui::Image((ImTextureID)m_SandboxLayer->Framebuffer.GetAttachment(0), {1920/4, 1080/4}, {0, 1}, {1, 0});
        ImGui::Image((ImTextureID)m_SandboxLayer->FramebufferOutline.GetAttachment(0), {1920/4, 1080/4}, {0, 1}, {1, 0});
        ImGui::Image((ImTextureID)m_SandboxLayer->FramebufferOutline.GetAttachment(1), {1920/4, 1080/4}, {0, 1}, {1, 0});
    }
    ImGui::End();

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    {
        ImGui::Checkbox("Demo Window", &ShowDemoWindow);      // Edit bools storing our window open/close state
        ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color
    }
    ImGui::End();

    m_PropertyEditor.Show(&m_ShowPropertyEditor);

}

void DebugLayer::OnEnd()
{
    
}


void DebugLayer::ShowFramerateOverlay(bool* show)
{
    static int location = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
    
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("FramerateOverlay", show, window_flags))
    {
        ImGui::Text("%.3f ms/f (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        
        if (ImGui::BeginPopupContextWindow())
        {
            if (show && ImGui::MenuItem("Close"))
                *show = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

bool DebugLayer::InputCaptured()
{
    // auto& io = ImGui::GetIO();
    return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGuizmo::IsOver();
}

} // namespace zrn