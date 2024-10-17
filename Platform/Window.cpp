#include "Window.hpp"
#include <GLFW/glfw3.h>

#include "Events.hpp"
// #include "Graphics/"

namespace zrn
{

void Window::Create(int width, int height, const std::string& title)
{
    ZRN_ON_DEBUG(
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    )

    m_GLFWWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    glfwSetWindowUserPointer(m_GLFWWindow, (void*)this);
    EventQueue::TrackWindow(*this);
}

void Window::Destroy()
{
    Close();
}

void Window::Close()
{
    glfwDestroyWindow(m_GLFWWindow);
}

bool Window::CloseRequested()
{
    return glfwWindowShouldClose(m_GLFWWindow);
}

std::pair<int, int> Window::GetFramebufferSize()
{
    int width, height;
    glfwGetWindowSize(m_GLFWWindow, &width, &height);
    return { width, height };
}

} // namespace zrn