#pragma once

#include "Common.hpp"

#include <stdint.h>
#include <queue>
#include <string>

namespace zrn
{

class Platform;
class Window;
class FileWatcher;

namespace EventData
{

struct Position
{
    int x, y;
};
struct Size
{
    int Width, Height;
};
struct Scroll
{
    double x, y;
};
struct Keyboard
{
    Keycode Key;
    Mod Mods;
};
struct Mouse
{
    MouseButton Button;
    Mod Mods;
};
struct DragPayload
{
    char** Paths;
    int Count;
};
struct Rename
{
    char OldName[260];
    char NewName[260];
};
struct File
{
    char Path[260];
};
}

struct Event
{
    EventType Type = EventType::None;
    Window* Window = nullptr;

union
{
    EventData::Position     Position;
    EventData::Size         Size;
    EventData::Scroll       Scroll;
    EventData::Keyboard     Keyboard;
    EventData::Mouse        Mouse;
    EventData::DragPayload  DragPayload;
    EventData::File         File;
    EventData::Rename       Rename;
};
};

class EventQueue
{
private:
    friend Window;
    friend FileWatcher;

public:
    EventQueue() = default;

    static bool Init(Platform* platform);
    static void Terminate();

    static bool Poll(Event& event);
    static void Push(Event&& event);
    static void Flush();

private:
    static void TrackWindow(Window& window);
    static void TrackFileWatcher(FileWatcher& watcher);

    static std::queue<Event> m_Queue;
    static Platform* m_Platform;
};

} // namespace zrn