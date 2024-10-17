#pragma once

#include <string>
#include "Common.hpp"
#include "Platform.hpp"
#include <string>

struct GLFWwindow;

namespace zrn
{

enum class WindowAttribute;

class EventQueue;
class Input;

class Window
{
private:
    friend Platform;
    friend EventQueue;
    friend Input;

public:
    Window() = default;

    void Create(int width = 1920, int height = 1080, const std::string& title = "Unnamed");
    void Destroy();

    // void Show();
    void Close();          // setwindowshouldclose
    bool CloseRequested(); // windowshouldclose
    
    std::string GetTitle(); // getwindowtitle
    void SetTitle(std::string title); // setwindowtitle
    // void SetIcon(Image)              // setwindowicon
    // void SetIcon(std::initializer_list<Image>)
    std::pair<int, int> GetPosition(); // getwindowpos
    void SetPosition(int x, int y); // setwindowpos
    std::pair<int, int> GetSize(); // getwindowsize
    void SetSize(int width, int height); // getwindowsize
    void SetSizeLimits(int min_width, int min_height, int max_width, int max_height); // setwindowsizelimits
    void SetWindowAspectRatio(int numerator, int denumerator); // setwindowaspectratio
    std::pair<int, int> GetFramebufferSize(); // getframebuffersize
    std::pair<float, float> GetContentScale();
    float GetOpacity();
    void SetOpacity(float opacity);
    
    void Minimize(); // iconifywindow
    void Restore(); // restorewindow
    void Maximize(); // maximizewindow
    void Show(); // showwindow
    void Hide(); // hidewindow
    void Focus(); // 
    void RequestAttention(); // requestwindowattention
    WindowAttribute GetAttributes();
    void SetAttribute();

    operator GLFWwindow* () { return m_GLFWWindow; }

private:
    GLFWwindow* m_GLFWWindow = nullptr;
};

} // namespace zrn