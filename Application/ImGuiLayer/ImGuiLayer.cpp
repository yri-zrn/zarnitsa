#include "ImGuiLayer.hpp"

#include "Window.hpp"
#include "Application.hpp"

// TEMP: for BeginPopupVoidTitle
#include "imgui_internal.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// #include "imgui_impl_opengl3_loader.h"

#include <GLFW/glfw3.h>

namespace zrn
{

namespace Internal
{

void _ImGuiBeginFrame()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void _ImGuiEndFrame()
{
    GLFWwindow* window = Layer::Application->Window;

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void _ImGuiInit()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();

    {
        auto& style = ImGuizmo::GetStyle();
        style.TranslationLineThickness   = 2.0f;
        style.TranslationLineArrowSize   = 5.0f;
        style.RotationLineThickness      = 2.0f;
        style.RotationOuterLineThickness = 2.0f;
        style.ScaleLineThickness         = 2.0f;
        style.ScaleLineCircleSize        = 5.0f;
        style.HatchedAxisLineThickness   = 5.0f;
        style.CenterCircleSize           = 5.0f;

        // initialize default colors
        style.Colors[ImGuizmo::DIRECTION_X]           = ImVec4(0.949f, 0.318f, 0.384f, 1.000f);
        style.Colors[ImGuizmo::DIRECTION_Y]           = ImVec4(0.549f, 0.824f, 0.263f, 1.000f);
        style.Colors[ImGuizmo::DIRECTION_Z]           = ImVec4(0.310f, 0.569f, 0.945f, 1.000f);
        style.Colors[ImGuizmo::PLANE_X]               = ImVec4(0.949f, 0.318f, 0.384f, 0.500f);
        style.Colors[ImGuizmo::PLANE_Y]               = ImVec4(0.549f, 0.824f, 0.263f, 0.500f);
        style.Colors[ImGuizmo::PLANE_Z]               = ImVec4(0.310f, 0.569f, 0.945f, 0.500f);
        style.Colors[ImGuizmo::SELECTION]             = ImVec4(0.900f, 0.900f, 0.900f, 0.900f);
        style.Colors[ImGuizmo::INACTIVE]              = ImVec4(0.600f, 0.600f, 0.600f, 0.600f);
        style.Colors[ImGuizmo::TRANSLATION_LINE]      = ImVec4(0.666f, 0.666f, 0.666f, 0.666f);
        style.Colors[ImGuizmo::SCALE_LINE]            = ImVec4(0.250f, 0.250f, 0.250f, 1.000f);
        style.Colors[ImGuizmo::ROTATION_USING_BORDER] = ImVec4(0.900f, 0.900f, 0.900f, 1.000f);
        style.Colors[ImGuizmo::ROTATION_USING_FILL]   = ImVec4(0.900f, 0.900f, 0.900f, 0.500f);
        style.Colors[ImGuizmo::HATCHED_AXIS_LINES]    = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
        style.Colors[ImGuizmo::TEXT]                  = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        style.Colors[ImGuizmo::TEXT_SHADOW]           = ImVec4(0.000f, 0.000f, 0.000f, 1.000f);
    }

    GLFWwindow* window = Layer::Application->Window;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::SetNextFrameWantCaptureKeyboard(false);
    ImGui::SetNextFrameWantCaptureMouse(false);
}

void _ImGuiTerminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

} // namespace Internal

} // namespace zrn


bool ImGui::BeginPopupVoidTitle(const char* name, ImGuiPopupFlags popup_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    
    
    int mouse_button = (popup_flags & ImGuiPopupFlags_MouseButtonMask_);
    
    if (IsMouseReleased(mouse_button) && !IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
        if (GetTopMostPopupModal() == NULL)
        {
            OpenPopupEx(GetID(name), popup_flags);
        }
    }

    if (!IsPopupOpen(GetID(name), ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
        return false;
    }

    ImGuiWindowFlags extra_window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    bool is_open = Begin(name, NULL, extra_window_flags | ImGuiWindowFlags_Popup);
    if (!is_open) // NB: Begin can return false when the popup is completely clipped (e.g. zero size display)
        EndPopup();

    return is_open;

    // if (IsMouseReleased(mouse_button) && !IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    //     if (GetTopMostPopupModal() == NULL)
    //         OpenPopupEx(id, popup_flags);
    // return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}