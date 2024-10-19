#include "Platform.hpp"

#include "Input.hpp"
#include "Window.hpp"
#include "Events.hpp"

#include "Internal/FileWatcher.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>

namespace zrn
{

bool Platform::Init()
{
    if (!glfwInit())
        return false;
    
    Input::Init(this);

    FileWatcherConfig config
    {
        "C:/dev/Zarnitsa/Resources",
        false,
        true
    };
    m_Watcher.Create(config);

    if (!EventQueue::Init(this))
        return false;

    return true;
}

void Platform::Terminate()
{
    EventQueue::Terminate();
    
    m_Watcher.Destroy();
    glfwTerminate();
}

void Platform::MakeContextCurrent(Window& window)
{
    glfwMakeContextCurrent(window.m_GLFWWindow);
    m_CurrentContext = &window;
}

Window* Platform::GetCurrentContext()
{
    // std::shared_ptr<Window> window;
    return m_CurrentContext;
}

void Platform::SwapBuffers(const Window& window)
{
    glfwSwapBuffers(window.m_GLFWWindow);
}

void Platform::SwapInterval(int interval)
{
    glfwSwapInterval(interval);
}

void Platform::PollEvents()
{
    // m_Watcher.Poll();
    glfwPollEvents();
}

Time Platform::GetTime()
{
    return glfwGetTime();
}

// uint64_t Platform::GetTimerFrequency()
// {
//     return glfwGetTimerFrequency();
// }

} // namespace zrn