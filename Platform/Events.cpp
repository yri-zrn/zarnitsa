#include "Events.hpp"

#include "Window.hpp"
#include "Platform.hpp"
#include "FileWatcher.hpp"

#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

namespace zrn
{

std::queue<Event> EventQueue::m_Queue;
Platform* EventQueue::m_Platform;

namespace
{
    void WindowPosCallback(GLFWwindow* window, int x, int y)
    {
        Event e = {};
        e.Type = EventType::WindowMoved;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Position = { x, y };
        EventQueue::Push(std::move(e));
    }
    void WindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        Event e = {};
        e.Type = EventType::WindowResized;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Size = { width, height };
        EventQueue::Push(std::move(e));
    }
    void WindowCloseCallback(GLFWwindow* window)
    {
        Event e = {};
        e.Type = EventType::WindowClosed;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void WindowRefreshCallback(GLFWwindow* window)
    {
        Event e = {};
        e.Type = EventType::WindowExposed;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void WindowFocusCallback(GLFWwindow* window, int focused)
    {
        Event e = {};
        e.Type = focused ? EventType::WindowFocusGained : EventType::WindowFocusLost;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void WindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        Event e = {};
        e.Type = iconified ? EventType::WindowMinimized : EventType::WindowRestored;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void WindowMaximizeCallback(GLFWwindow* window, int maximized)
    {
        Event e = {};
        e.Type = maximized ? EventType::WindowMaximized : EventType::WindowRestored;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // Mod mod = Mod(mods & GLFW_MOD_SHIFT | mods & GLFW_MOD_CONTROL | mods & GLFW_MOD_ALT
        //             | mods & GLFW_MOD_SUPER | mods & GLFW_MOD_CAPS_LOCK | mods & GLFW_MOD_NUM_LOCK);

        Event e = {};
        e.Type = action == GLFW_RELEASE ? EventType::MouseButtonUp : EventType::MouseButtonDown;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Mouse.Button = (MouseButton)button;
        e.Mouse.Mods = (Mod)mods;
        EventQueue::Push(std::move(e));
    }
    void MouseCursorPosCallback(GLFWwindow* window, double x, double y)
    {
        Event e = {};
        e.Type = EventType::MouseMoved;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Position = { (int)x, (int)y };
        EventQueue::Push(std::move(e));
    }
    void MouseCursorEnterCallback(GLFWwindow* window, int entered)
    {
        Event e = {};
        e.Type = entered ? EventType::WindowMouseEnter : EventType::WindowMouseLeave;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        EventQueue::Push(std::move(e));
    }
    void ScrollCallback(GLFWwindow* window, double x, double y)
    {
        Event e = {};
        e.Type = EventType::MouseScrolled;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Scroll = { x, y };
        EventQueue::Push(std::move(e));
    }
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Event e = {};
        if (action == GLFW_PRESS)
            e.Type = EventType::KeyPressed;
        else if (action == GLFW_RELEASE)
            e.Type = EventType::KeyReleased;
        else if (action == GLFW_REPEAT)
            e.Type = EventType::KeyRepeated;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        e.Keyboard.Key = (Keycode)key;
        EventQueue::Push(std::move(e));
    }
    void CharCallback(GLFWwindow* window, unsigned int codepoint)
    {
        Event e = {};
        e.Type = EventType::TextInput;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        // e.
        assert(false);
        // TODO: add char callback
    }
    void FileDropCallback(GLFWwindow* window, int count, const char** paths)
    {
        Event e = {};
        e.Type = EventType::FileDropped;
        e.Window = (Window*)glfwGetWindowUserPointer(window);
        // e.File.Paths = (char**)
        // TODO: add drop callback
        assert(false);
    }

    void FileCreatedCallback(Internal::FileWatcher* watcher, const char* path)
    {
        Event e = {};
        e.Type = EventType::FileCreated;
        strcpy_s(e.File.Path, sizeof(e.File.Path), path);
        EventQueue::Push(std::move(e));
    }

    void FileDeletedCallback(Internal::FileWatcher* watcher, const char* path)
    {
        Event e = {};
        e.Type = EventType::FileDeleted;
        strcpy_s(e.File.Path, sizeof(e.File.Path), path);
        EventQueue::Push(std::move(e));
    }

    void FileModifiedCallback(Internal::FileWatcher* watcher, const char* path)
    {
        Event e = {};
        e.Type = EventType::FileModified;
        strcpy_s(e.File.Path, sizeof(e.File.Path), path);
        EventQueue::Push(std::move(e));
    }

    void FileRenamedCallback(Internal::FileWatcher* watcher, const char* old_name, const char* new_name)
    {
        Event e = {};
        e.Type = EventType::FileRenamed;
        strcpy_s(e.Rename.NewName, sizeof(e.File.Path), old_name);
        strcpy_s(e.Rename.OldName, sizeof(e.File.Path), new_name);
        EventQueue::Push(std::move(e));
    }
}

bool EventQueue::Init(Platform* platform)
{
    m_Platform = platform;

    TrackFileWatcher(m_Platform->m_Watcher);
    
    std::thread watcher_thread{[&](){
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            m_Platform->m_Watcher.Poll();
        }
    }};

    watcher_thread.detach();

    return true;
}

void EventQueue::Terminate()
{
    
}

bool EventQueue::Poll(Event& event)
{
    if (m_Queue.empty())
        return false;
    
    event = m_Queue.front();
    m_Queue.pop();
    
    return true;
}

void EventQueue::Push(Event&& event)
{
    m_Queue.emplace(event);
}

void EventQueue::Flush()
{
    glfwPollEvents();
    // m_Platform->m_Watcher.Poll();
}

void EventQueue::TrackWindow(Window& window)
{
    GLFWwindow* _window = window.m_GLFWWindow;
    glfwSetWindowPosCallback(_window, WindowPosCallback);
    glfwSetWindowSizeCallback(_window, WindowSizeCallback);
    glfwSetWindowCloseCallback(_window, WindowCloseCallback);
    glfwSetWindowRefreshCallback(_window, WindowRefreshCallback);
    glfwSetWindowFocusCallback(_window, WindowFocusCallback);
    glfwSetWindowIconifyCallback(_window, WindowIconifyCallback);
    glfwSetMouseButtonCallback(_window, MouseButtonCallback);
    glfwSetCursorPosCallback(_window, MouseCursorPosCallback);
    glfwSetCursorEnterCallback(_window, MouseCursorEnterCallback);
    glfwSetScrollCallback(_window, ScrollCallback);
    glfwSetKeyCallback(_window, KeyCallback);
}

void EventQueue::TrackFileWatcher(FileWatcher& watcher)
{
    watcher.SetFileCreatedCallback(FileCreatedCallback);
    watcher.SetFileDeletedCallback(FileDeletedCallback);
    watcher.SetFileModifiedCallback(FileModifiedCallback);
    watcher.SetFileRenamedCallback(FileRenamedCallback);
}

} // namespace zrn