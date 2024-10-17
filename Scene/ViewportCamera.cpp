#include "ViewportCamera.hpp"

#include "Common/Timestep.hpp"
#include "Platform/Events.hpp"
#include "Platform/Input.hpp"

#include <iostream>

namespace zrn
{

ViewportCamera::ViewportCamera()
{
    UpdateProjection();
    UpdatePosition();
    UpdateView();
    UpdatePanSpeed();
    UpdateZoomSpeed();
}

void ViewportCamera::OnUpdate(Timestep step)
{
    glm::vec2 mouse_pos = Input::GetMousePosition();
    glm::vec2 delta = mouse_pos - m_InitialMousePos;
    m_InitialMousePos = mouse_pos;
    delta *= step;

    const float threshold = 1e-5f;
    if (glm::length(delta) > threshold)
    {
        if (Input::IsMouseButtonPressed(MouseButton::Middle))
        {
            if (Input::IsKeyPressed(Keycode::LeftShift))
                MousePan(delta);
            else if (Input::IsKeyPressed(Keycode::LeftControl))
                MouseZoom(-delta.y);
            else
                MouseRotate(delta);
        }
    }
}

void ViewportCamera::OnEvent(Event event)
{
    if (event.Type == EventType::MouseScrolled)
    {
        float delta = (float)event.Scroll.y * 0.05f;
        MouseZoom(delta);
    }
    if (event.Type == EventType::WindowResized)
    {
        OnViewportResize(event.Size.Width, event.Size.Height);
    }
}

void ViewportCamera::OnViewportResize(uint32_t width, uint32_t height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    AspectRatio = m_ViewportWidth / m_ViewportHeight;
    
    UpdatePanSpeed();
    UpdateProjection();
}

glm::quat ViewportCamera::Orientation() const
{
    return glm::quat(glm::vec3(-Pitch, -Yaw, 0.0f));
}

glm::vec3 ViewportCamera::Forward() const
{
    return glm::rotate(Orientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 ViewportCamera::Right() const
{
    return glm::rotate(Orientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 ViewportCamera::Up() const
{
    return glm::rotate(Orientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void ViewportCamera::UpdatePosition()
{
    Position = m_OriginPosition - Forward() * m_DistanceToOrigin;
}

void ViewportCamera::UpdateProjection()
{
    Projection = glm::perspective(glm::radians(FOV), AspectRatio, NearClip, FarClip);
    ViewProjection = Projection * View;
}

void ViewportCamera::UpdateView()
{
    glm::quat orientation = Orientation();
    View = glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(orientation);
    View = glm::inverse(View);

    ViewProjection = Projection * View;
}

void ViewportCamera::MousePan(glm::vec2 delta)
{
    m_OriginPosition += -Right() * delta.x * m_PanSpeed.x * m_DistanceToOrigin;
    m_OriginPosition += Up() * delta.y * m_PanSpeed.y * m_DistanceToOrigin;

    UpdatePosition();
    UpdateView();
}

void ViewportCamera::MouseRotate(glm::vec2 delta)
{
    float yaw_sign = Up().y < 0 ? -1.0f : 1.0f;
    Yaw += yaw_sign * delta.x * m_RotationSpeed;
    Pitch += delta.y * m_RotationSpeed;

    UpdatePosition();
    UpdateView();
}

void ViewportCamera::MouseZoom(float delta)
{
    m_DistanceToOrigin -= delta * m_ZoomSpeed;
    UpdateZoomSpeed();
    UpdatePosition();
    UpdateView();
}

void ViewportCamera::UpdatePanSpeed()
{
    const float max_pan_speed = 2.4f;

    float x = std::min(m_ViewportWidth / 1000.0f, max_pan_speed);
    float y = std::min(m_ViewportHeight / 1000.0f, max_pan_speed);

    m_PanSpeed.x = 0.0366f * x * x - 0.1778f * x + 0.3021f;
    m_PanSpeed.y = 0.0366f * y * y - 0.1778f * y + 0.3021f;
}

void ViewportCamera::UpdateZoomSpeed()
{
    float min_distance = m_DistanceToOrigin * 0.2f;
    min_distance = std::max(m_DistanceToOrigin, 0.0f);
    float speed = min_distance * min_distance;
    const float max_zoom_speed = 100.0f;
    m_ZoomSpeed = std::min(speed, max_zoom_speed);
}

} // namespace zrn