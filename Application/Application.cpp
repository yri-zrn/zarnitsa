#include "Application.hpp"

#include "Common/Common.hpp"
#include "Events.hpp"

#include <string>
#include <iostream>
#include <filesystem>

namespace zrn
{

std::string GetEventString(Event& event)
{
    #define SWITCH_EVENT(e) case EventType::e: return #e;
    
    switch (event.Type)
    {
        SWITCH_EVENT(WindowShown)
        SWITCH_EVENT(WindowHidden)
        SWITCH_EVENT(WindowExposed)
        SWITCH_EVENT(WindowMoved)
        SWITCH_EVENT(WindowResized)
        SWITCH_EVENT(WindowPixelSizeChanged)
        SWITCH_EVENT(WindowMinimized)
        SWITCH_EVENT(WindowMaximized)
        SWITCH_EVENT(WindowRestored)
        SWITCH_EVENT(WindowMouseEnter)
        SWITCH_EVENT(WindowMouseLeave)
        SWITCH_EVENT(WindowFocusGained)
        SWITCH_EVENT(WindowFocusLost)
        SWITCH_EVENT(WindowClosed)
        SWITCH_EVENT(WindowHitTest)
        SWITCH_EVENT(WindowDisplayChanged)
        SWITCH_EVENT(WindowDisplayScaleChanged)
        SWITCH_EVENT(WindowOccluded)
        SWITCH_EVENT(WindowEnterFullscreen)
        SWITCH_EVENT(WindowLeaveFullscreen)
        SWITCH_EVENT(KeyPressed)
        SWITCH_EVENT(KeyReleased)
        SWITCH_EVENT(KeyRepeated)
        SWITCH_EVENT(TextEditing)
        SWITCH_EVENT(TextInput)
        SWITCH_EVENT(KeymapChanged)
        SWITCH_EVENT(MouseMoved)
        SWITCH_EVENT(MouseButtonDown)
        SWITCH_EVENT(MouseButtonUp)
        SWITCH_EVENT(MouseScrolled)
        SWITCH_EVENT(FileDropped)
    default: return "";

    #undef SWITCH_EVENT
    };
}

std::string GetKeyString(Keycode key)
{
    #define SWITCH_KEY(k) case Keycode::k: return #k;
    
    switch (key)
    {
        SWITCH_KEY(None)
        SWITCH_KEY(Space)
        SWITCH_KEY(Apostrophe)
        SWITCH_KEY(Comma)
        SWITCH_KEY(Minus)
        SWITCH_KEY(Period)
        SWITCH_KEY(Slash)
        SWITCH_KEY(_0)
        SWITCH_KEY(_1)
        SWITCH_KEY(_2)
        SWITCH_KEY(_3)
        SWITCH_KEY(_4)
        SWITCH_KEY(_5)
        SWITCH_KEY(_6)
        SWITCH_KEY(_7)
        SWITCH_KEY(_8)
        SWITCH_KEY(_9)
        SWITCH_KEY(Semicolon)
        SWITCH_KEY(Equal)
        SWITCH_KEY(A)
        SWITCH_KEY(B)
        SWITCH_KEY(C)
        SWITCH_KEY(D)
        SWITCH_KEY(E)
        SWITCH_KEY(F)
        SWITCH_KEY(G)
        SWITCH_KEY(H)
        SWITCH_KEY(I)
        SWITCH_KEY(J)
        SWITCH_KEY(K)
        SWITCH_KEY(L)
        SWITCH_KEY(M)
        SWITCH_KEY(N)
        SWITCH_KEY(O)
        SWITCH_KEY(P)
        SWITCH_KEY(Q)
        SWITCH_KEY(R)
        SWITCH_KEY(S)
        SWITCH_KEY(T)
        SWITCH_KEY(U)
        SWITCH_KEY(V)
        SWITCH_KEY(W)
        SWITCH_KEY(X)
        SWITCH_KEY(Y)
        SWITCH_KEY(Z)
        SWITCH_KEY(LeftBracket)
        SWITCH_KEY(BackSlash)
        SWITCH_KEY(RightBracket)
        SWITCH_KEY(GraveAccent)
        SWITCH_KEY(World1)
        SWITCH_KEY(World2)
        SWITCH_KEY(Escape)
        SWITCH_KEY(Enter)
        SWITCH_KEY(Tab)
        SWITCH_KEY(Backspace)
        SWITCH_KEY(Insert)
        SWITCH_KEY(Delete)
        SWITCH_KEY(Right)
        SWITCH_KEY(Left)
        SWITCH_KEY(Down)
        SWITCH_KEY(Up)
        SWITCH_KEY(PageUp)
        SWITCH_KEY(PageDown)
        SWITCH_KEY(Home)
        SWITCH_KEY(End)
        SWITCH_KEY(CapsLock)
        SWITCH_KEY(ScrollLock)
        SWITCH_KEY(NumLock)
        SWITCH_KEY(PrintScreen)
        SWITCH_KEY(Pause)
        SWITCH_KEY(F1)
        SWITCH_KEY(F2)
        SWITCH_KEY(F3)
        SWITCH_KEY(F4)
        SWITCH_KEY(F5)
        SWITCH_KEY(F6)
        SWITCH_KEY(F7)
        SWITCH_KEY(F8)
        SWITCH_KEY(F9)
        SWITCH_KEY(F10)
        SWITCH_KEY(F11)
        SWITCH_KEY(F12)
        SWITCH_KEY(F13)
        SWITCH_KEY(F14)
        SWITCH_KEY(F15)
        SWITCH_KEY(F16)
        SWITCH_KEY(F17)
        SWITCH_KEY(F18)
        SWITCH_KEY(F19)
        SWITCH_KEY(F20)
        SWITCH_KEY(F21)
        SWITCH_KEY(F22)
        SWITCH_KEY(F23)
        SWITCH_KEY(F24)
        SWITCH_KEY(F25)
        SWITCH_KEY(KP_0)
        SWITCH_KEY(KP_1)
        SWITCH_KEY(KP_2)
        SWITCH_KEY(KP_3)
        SWITCH_KEY(KP_4)
        SWITCH_KEY(KP_5)
        SWITCH_KEY(KP_6)
        SWITCH_KEY(KP_7)
        SWITCH_KEY(KP_8)
        SWITCH_KEY(KP_9)
        SWITCH_KEY(KP_Decimal)
        SWITCH_KEY(KP_Divide)
        SWITCH_KEY(KP_Multiply)
        SWITCH_KEY(KP_Subtract)
        SWITCH_KEY(KP_Add)
        SWITCH_KEY(KP_Enter)
        SWITCH_KEY(KP_Equal)
        SWITCH_KEY(LeftShift)
        SWITCH_KEY(LeftControl)
        SWITCH_KEY(LeftAlt)
        SWITCH_KEY(LeftSuper)
        SWITCH_KEY(RightShift)
        SWITCH_KEY(RightControl)
        SWITCH_KEY(RightAlt)
        SWITCH_KEY(RightSuper)
        SWITCH_KEY(Menu)
    }

    return "None";
    #undef SWITCH_KEY
}

bool Application::Init()
{
    Log::Init();

    if (!Platform.Init())
        return false;

    Window.Create();
    Platform.MakeContextCurrent(Window);
    Platform.SwapInterval(1);

    for (auto layer : m_LayerStack)
    {
        if (!layer->_Init(layer))
            return false;
    }
    
    if (!AssetManager.Init(&Context))
        return false;

    AssetManager.LoadResources(&Context);

    return true;
}

void Application::Terminate()
{
    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        (*it)->_OnEnd(*it);

    Platform.Terminate();
}

void Application::Run()
{
    OnBegin();
    while (!m_CloseRequested)
    {
        static bool window_minimized = false;

        Event event;
        while (EventQueue::Poll(event))
        {
            if (event.Type == EventType::WindowClosed)
            {
                if (event.Window == &Window)
                    m_CloseRequested = true;
            }
            else if (event.Type == EventType::WindowMinimized)
            {
                window_minimized = true;
            }
            else if (event.Type == EventType::WindowMaximized
                  || event.Type == EventType::WindowRestored)
            {
                window_minimized = false;
            }
            else if (event.Type == EventType::WindowResized)
            {
                if (event.Size.Height == 0 || event.Size.Width == 0)
                    window_minimized = true;
                else
                    window_minimized = false;
            }
            else if (event.Type == EventType::FileCreated)
            {
                std::cout << "File created " << event.File.Path << '\n';
                AssetManager.TryLoadResource(&Context, event.File.Path);
            }
            else if (event.Type == EventType::FileDeleted)
            {
                std::cout << "File deleted " << event.File.Path << '\n';
            }
            else if (event.Type == EventType::FileModified)
            {
                std::cout << "File modified " << event.File.Path << '\n';
                
                // auto shader_name = std::filesystem::path(event.File.Path).stem().string();
                // if (auto shader = AssetManager.GetShader(shader_name))
                // {
                //     AssetManager.LoadShader(&Context, event.File.Path);
                // }
                AssetManager.TryLoadResource(&Context, event.File.Path);
            }
            else if (event.Type == EventType::FileRenamed)
            {
                std::cout << "File renamed " << event.Rename.NewName << " <- " << event.Rename.OldName << '\n';
                AssetManager.TryLoadResource(&Context, event.Rename.NewName);
            }

            if (!window_minimized)
                OnEvent(event);
        }

        if (m_CloseRequested)
            break;

        Time now = Platform.GetTime();
        Timestep timestep = (now - m_LastTime);
        m_LastTime = now;
        
        if (!window_minimized)
        {
            OnUpdate(timestep);
            OnImGuiRender();
            Platform.SwapBuffers(Window);
        }

        EventQueue::Flush();
        // m_Platform.PollEvents();
        
    }
    OnEnd();
}

void Application::Close()
{

}

void Application::OnBegin()
{
    for (auto layer : m_LayerStack)
        layer->_OnBegin(layer);
}

void Application::OnUpdate(Timestep step)
{
    for (auto layer : m_LayerStack)
        layer->_OnUpdate(layer, step);
}

void Application::OnEvent(Event event)
{
    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        (*it)->_OnEvent(*it, event);
}

void Application::OnImGuiRender()
{
    for (auto layer : m_LayerStack)
        layer->_OnImGuiRender(layer);
}

void Application::OnEnd()
{
    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        (*it)->_OnEnd(*it);
}


} // namespace zrn