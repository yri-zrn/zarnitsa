#include "Application.hpp"

#include "Sandbox/SandboxLayer.hpp"
#include "Sandbox/DebugLayer.hpp"

#include <windows.h>

#ifdef ZRN_DEBUG
    #define ZRN_ON_DEBUG(x) x
    #define ZRN_UNUSED(var) ((void)var);
#else
    #define ZRN_ON_DEBUG(x)
    #define ZRN_UNUSED(var)
#endif

/*

? Known issues:
    - Possible FileModified event dublication (caused in FileWatcherWin32.cpp)
        - Temporary fixed by making watcher thread sleep for 300 ms after polling (Events.cpp)
        - Watcher thread can cause segfault at application exit (it adresses EventQueue after
          is was deconstructed because main() finished)
        - Assertion failed: result, file C:/Zarnitsa/Platform/Internal/Win32/FileWatcherWin32.cpp, line 202
    - Hard-coded Resource folder (Platform.cpp)
    - Parent objects unable to skew children on scaling
    - CMakeLists.txt uses $ENV{VULKAN_SDK} for linking shaderc
    - ImGuizmo reacts on key events event when they should be handled by ImGui
!   - All OpenGL resourses that allocates memory do not free it (should add delete functions for resources)
    - Should remove shaderc from dependencies
    - Shader run-time recompilation is slow, because it should run on different thread

    - Logger must be initialized before any system because all systems rely on it

? TODO:
    - Add compute and geometry shader support
    - Add Unbind() functions for graphics resources
    - Add sleep in main loop for achieving necessary fps amount
    - All vertex layout stuff should be removed from vertex buffer
    - Add Status util class for retrieving result of function call
    - Should make all resource creation functions return Status instead of bool

    - Move Timestep to Application
    - Fix Mesh Loader : it load amount of vertices tripled
    - Make Uniform and Vertex buffers template?

? In progress:

? Notes:
    - material can have shader and uniform buffer 

*/
    
/*

Current Shader Pipeline

                Shader Macro Definitions
FRAGMENT_SHADER
VERTEX_SHADER
TODO: add VK_GLSL and GL_GLSL

                Shader Layout
Binding     Description                 Size
0           Push constant buffer        128
1           Scene data                  TODO: add
2           Model data / material data  TODO: add


binding 0 -> Push Constant Buffer (size)

*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
// int main()
{
    ZRN_UNUSED(hInstance);
    ZRN_UNUSED(hPrevInstance);
    ZRN_UNUSED(lpCmdLine);
    ZRN_UNUSED(nShowCmd);

    zrn::Application app;
    zrn::SandboxLayer sandbox_layer;
    zrn::DebugLayer debug_layer;
    app.Attach("SandboxLayer", sandbox_layer);
    app.Attach("DebugLayer", debug_layer);

    app.Init();
    app.Run();
    app.Terminate();

    return EXIT_SUCCESS;
}