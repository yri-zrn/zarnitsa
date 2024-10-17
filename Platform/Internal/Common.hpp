#pragma once

#include <stdint.h>
#include <assert.h>

namespace zrn
{

enum class PlatformID { None, Win32 };

enum class MouseButton : int16_t;
enum class Keycode : int16_t ;
enum class Action : int16_t ;
enum class Mod : int16_t ;
typedef int16_t Scancode;

struct WindowConfig;
struct ContextConfig;
struct FramebufferConfig;

namespace Internal
{
struct TLS;
struct Mutex;
struct AtomicInt;
struct Timer;
struct Window;
struct Context;
struct FileWatcher;

typedef void (*Proc)();

} // namespace Internal

#ifndef ERROR_MESSAGE_DEFINED
#define ERROR_MESSAGE_DEFINED
struct Error
{
    static const size_t MessageSize = 1024; 
    Error* Next;
    char   Description[MessageSize];
};
typedef void (*ErrorCallback)(const char* description);

#endif

struct WindowConfig
{
    WindowConfig() = default;

    int  x                = -1; // Any
    int  y                = -1;
    int  Width            = 0;
    int  Height           = 0;
    const char* Title     = "Unnamed";
    bool Resizable        = true;
    bool Visible          = true;
    bool Fullscreen       = false;
    bool Decorated        = true;
    bool Focused          = true;
    bool AutoMinimize     = false;
    bool Floating         = false;
    bool Maximized        = false;
    bool CenterCursor     = true;
    bool FocusOnShow      = true;
    bool MousePassthrough = false;
    bool ScaleToMonitor   = false;
    bool ScaleFramebuffer = true;
    int  RefreshRate      = -1;
};

struct ContextConfig
{
    ContextConfig() = default;

    int  Major      = 3;
    int  Minor      = 0;
    bool Forward    = false;
    bool Debug      = false;
    bool NoError    = false;
    int  Profile    = 0;
    int  Robustness = 0;
    int  Release    = 0;
};

struct FramebufferConfig
{
    FramebufferConfig() = default;

    int         RedBits         = 8;
    int         GreenBits       = 8;
    int         BlueBits        = 8;
    int         AlphaBits       = 8;
    int         DepthBits       = 24;
    int         StencilBits     = 8;
    int         AccumRedBits    = 0;
    int         AccumGreenBits  = 0;
    int         AccumBlueBits   = 0;
    int         AccumAlphaBits  = 0;
    int         AuxBuffers      = 0;
    bool        Stereo          = false;
    int         Samples         = 0;
    bool        sRGB            = false;
    bool        Doublebuffer    = true;
    bool        Transparent     = false;
    uintptr_t   Handle          = 0;
};

struct FileWatcherConfig
{
    FileWatcherConfig() = default;

    const char* Path = nullptr;
    bool Blocking  = false;
    bool Recursive = true;
};

enum class OpenGLProfile
{
    Any           = 0,
    Core          = 0x00032001,
    Compatibility = 0x00032002
};

enum class RobustnessStrategy
{
    None                = 0,
    NoResetNotification = 0x00031001,
    LoseContextOnReset  = 0x00031002
};

enum class ReleaseBehavior
{
    None      = 0,
    Flush
};

enum class CursorMode
{
    Normal,
    Hidden,
    Disabled,
    Captured
};

enum class MouseButton : int16_t {
    None   = -1,
    _1     =  0,
    _2     =  1,
    _3     =  2,
    _4     =  3,
    _5     =  4,
    _6     =  5,
    _7     =  6,
    _8     =  7,
    Last   = _8,
    Left   = _1,
    Right  = _2,
    Middle = _3
};

enum class Keycode : int16_t {
    None            = -1,
    Space           = 32,
    Apostrophe      = 39,
    Comma           = 42,
    Minus           = 45,
    Period          = 46,
    Slash           = 47,
    _0              = 48,
    _1              = 49,
    _2              = 50,
    _3              = 51,
    _4              = 52,
    _5              = 53,
    _6              = 54,
    _7              = 55,
    _8              = 56,
    _9              = 57,
    Semicolon       = 59,
    Equal           = 61,
    A               = 65,
    B               = 66,
    C               = 67,
    D               = 68,
    E               = 69,
    F               = 70,
    G               = 71,
    H               = 72,
    I               = 73,
    J               = 74,
    K               = 75,
    L               = 76,
    M               = 77,
    N               = 78,
    O               = 79,
    P               = 80,
    Q               = 81,
    R               = 82,
    S               = 83,
    T               = 84,
    U               = 85,
    V               = 86,
    W               = 87,
    X               = 88,
    Y               = 89,
    Z               = 90,
    LeftBracket     = 91,
    BackSlash       = 92,
    RightBracket    = 93,
    GraveAccent     = 96,
    World1          = 161,
    World2          = 162,
    Escape          = 256,
    Enter           = 257,
    Tab             = 258,
    Backspace       = 259,
    Insert          = 260,
    Delete          = 261,
    Right           = 262,
    Left            = 263,
    Down            = 264,
    Up              = 265,
    PageUp          = 266,
    PageDown        = 267,
    Home            = 268,
    End             = 269,
    CapsLock        = 280,
    ScrollLock      = 281,
    NumLock         = 282,
    PrintScreen     = 283,
    Pause           = 284,
    F1              = 290,
    F2              = 291,
    F3              = 292,
    F4              = 293,
    F5              = 294,
    F6              = 295,
    F7              = 296,
    F8              = 297,
    F9              = 298,
    F10             = 299,
    F11             = 300,
    F12             = 301,
    F13             = 302,
    F14             = 303,
    F15             = 304,
    F16             = 305,
    F17             = 306,
    F18             = 307,
    F19             = 308,
    F20             = 309,
    F21             = 310,
    F22             = 311,
    F23             = 312,
    F24             = 313,
    F25             = 314,
    KP_0            = 320,
    KP_1            = 321,
    KP_2            = 322,
    KP_3            = 323,
    KP_4            = 324,
    KP_5            = 325,
    KP_6            = 326,
    KP_7            = 327,
    KP_8            = 328,
    KP_9            = 329,
    KP_Decimal      = 330,
    KP_Divide       = 331,
    KP_Multiply     = 332,
    KP_Subtract     = 333,
    KP_Add          = 334,
    KP_Enter        = 335,
    KP_Equal        = 336,
    LeftShift       = 340,
    LeftControl     = 341,
    LeftAlt         = 342,
    LeftSuper       = 343,
    RightShift      = 344,
    RightControl    = 345,
    RightAlt        = 346,
    RightSuper      = 347,
    Menu            = 348,

    First           = Space,
    Last            = Menu
};

enum class Action : int16_t {
    None      = -1,
    Press,
    Release,
    Repeat
};

enum class Mod : int16_t {
    None     = 0,
    Shift    = 0x001,
    Control  = 0x002,
    Alt      = 0x004,
    Super    = 0x008,
    CapsLock = 0x010,
    NumLock  = 0x020
};

inline constexpr enum Mod operator |(const enum Mod lhs, const enum Mod rhs)
{
    return (enum Mod)(uint32_t(lhs) | uint32_t(rhs));
}

inline constexpr enum Mod& operator|= (enum Mod& lhs, const enum Mod& rhs)
{
    return lhs = lhs | rhs;
}

struct Event;

enum class EventType : uint32_t
{
    None = 0,
    ApplicationQuit,

    LocaleChanged,
    SystemThemeChanged,
    ClipboardUpdate,

    WindowShown,
    WindowHidden,
    WindowExposed,
    WindowMoved,
    WindowResized,
    WindowPixelSizeChanged,
    WindowMinimized,
    WindowMaximized,
    WindowRestored,
    WindowMouseEnter,
    WindowMouseLeave,
    WindowFocusGained,
    WindowFocusLost,
    WindowClosed,
    WindowHitTest,
    WindowDisplayChanged,
    WindowDisplayScaleChanged,
    WindowOccluded,
    WindowEnterFullscreen,
    WindowLeaveFullscreen,
    
    KeyPressed,
    KeyReleased,
    KeyRepeated,
    TextEditing,
    TextInput,
    KeymapChanged,
    
    MouseMoved,
    MouseButtonDown,
    MouseButtonUp,
    MouseScrolled,

    FileDropped,

    FileRenamed,
    FileDeleted,
    FileModified,
    FileCreated
};

enum class EventAction
{
    AddEvent,
    PeekEvent,
    GetEvent
};

typedef void (*EventCallback)(Event& event);

typedef bool (*EventFilter)(void* userdata, Event* event);

} // namespace zrn