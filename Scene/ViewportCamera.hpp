#pragma once

#include "Common/Math.hpp"

namespace zrn
{

struct Event;
class Timestep;

class ViewportCamera
{
public:
    ViewportCamera();

    void OnUpdate(Timestep step);
    void OnEvent(Event event);
    void OnViewportResize(uint32_t width, uint32_t height);

    float FOV = 45.0f;
    float AspectRatio = 16.0f / 9.0f;
    float NearClip = 0.01f;
    float FarClip = 1000.0f;

    float Pitch = 120.0f;
    float Yaw = 45.0f;

    glm::vec3 Position;

    glm::mat4 View {1.0f};
    glm::mat4 ViewProjection {1.0f};
    glm::mat4 Projection {1.0f};

    glm::quat Orientation() const;
    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;
    glm::vec3 Origin() const { return m_OriginPosition; }

private:
    void UpdatePosition();
    void UpdateProjection();
    void UpdateView();

private:
    void MousePan(glm::vec2 delta);
    void MouseRotate(glm::vec2 delta);
    void MouseZoom(float delta);

    void UpdateZoomSpeed();
    void UpdatePanSpeed();
    
private:
    glm::vec3 m_OriginPosition= { 0.0f, 0.0f, 0.0f };
    float m_DistanceToOrigin = 16.0f;


private:
    glm::vec2 m_InitialMousePos = { 0.0f, 0.0f };

    glm::vec2 m_PanSpeed;
    float m_RotationSpeed = 0.8f;
    float m_ZoomSpeed;

    float m_ViewportWidth, m_ViewportHeight;
};

} // namespace zrn