#pragma once

#include "Common.hpp"
#include "FileWatcher.hpp"
#include <memory>

namespace zrn
{

class Window;
class EventQueue;

class Platform
{
private:
    friend EventQueue;

public:
    Platform() = default;

    bool Init();
    void Terminate();

    void MakeContextCurrent(Window&);
    Window* GetCurrentContext();
    void SwapBuffers(const Window&);
    void SwapInterval(int interval);
    void PollEvents();

    Time GetTime();
    // uint64_t GetTimerFrequency();

private:
    Window* m_CurrentContext = nullptr;
    FileWatcher m_Watcher;
};

} // namespace zrn