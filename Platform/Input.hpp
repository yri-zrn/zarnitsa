#pragma once

#include "Common.hpp"

namespace zrn
{

class Platform;

// TODO: process events of focused window, not the main one
class Input
{
public:
    static bool Init(Platform* platform);
    static void Terminate();

    static bool IsKeyPressed(Keycode key);
    static bool IsKeyDown(Keycode key);
    static bool IsKeyReleased(Keycode key);
    
    static bool IsMouseButtonPressed(MouseButton button);
    static glm::vec2 GetMousePosition();

private:
    static Platform* m_Platform;
};

} // namespace zrn