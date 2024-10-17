#include "Input.hpp"

#include "Platform.hpp"
#include "Window.hpp"

#include <GLFW/glfw3.h>

namespace zrn
{

Platform* Input::m_Platform = nullptr;

bool Input::Init(Platform* platform)
{
    m_Platform = platform;
    return true;
}

void Input::Terminate()
{
    m_Platform = nullptr;
}

bool Input::IsKeyPressed(Keycode key)
{
    assert(m_Platform);
    
    GLFWwindow* window = m_Platform->GetCurrentContext()->m_GLFWWindow;
    return glfwGetKey(window, (int)key);
}

// bool Input::KeyReleased(Keycode key)
// {
//     GLFWwindow* window = m_Platform->GetCurrentContext()->m_GLFWWindow;
//     return glfw
// }

bool Input::IsMouseButtonPressed(MouseButton button)
{
    assert(m_Platform);

    GLFWwindow* window = m_Platform->GetCurrentContext()->m_GLFWWindow;
    return glfwGetMouseButton(window, (int)button) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
    assert(m_Platform);

    GLFWwindow* window = m_Platform->GetCurrentContext()->m_GLFWWindow;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return {(float)x, (float)y};
}

} // namespace zrn