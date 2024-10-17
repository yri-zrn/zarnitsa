#include "GraphicsDevice.hpp"

#if defined(ZRN_WIN32)
    #include "windows.h"
#endif

#include "Internal.hpp"
#include "OpenGL/GraphicsDeviceOpenGL.hpp"

namespace zrn::Internal
{

bool GraphicsDevice::Init(GraphicsAPI desired_api, GraphicsProc procedure)
{
    ZRN_CORE_ASSERT(desired_api != GraphicsAPI::None);

    if (_Initialized)
        return true;
    
    _OpenGLSupported = _CheckOpenGLSupport();
    
    if (!Connect(desired_api))
    {
        ZRN_CORE_ERROR("Failed to connect desired API: {0}", _GetGraphicsAPIString(desired_api));
        return false;
    }
    
    if (!InitDevice(procedure))
    {
        ZRN_CORE_ERROR("Failed to initialize graphics device: {0}", _GetGraphicsAPIString(desired_api));
        return false;
    }

    _Initialized = true;
    return true;
}

void GraphicsDevice::Terminate()
{
    if (!_Initialized)
        return;
    
    TerminateDevice();
}

bool GraphicsDevice::Connect(GraphicsAPI desired_api)
{
    if (desired_api == GraphicsAPI::OpenGL)
    {
        if (!_OpenGLSupported)
        {
            ZRN_CORE_ERROR("OpenGL not supported");
            return false;
        }

        _API = GraphicsAPI::OpenGL;
        return GraphicsDeviceOpenGL::Connect(this);
    }

    assert(false);
    return false;
}

bool GraphicsDevice::_CheckOpenGLSupport()
{
    bool supported = false;

#if defined(ZRN_WIN32)
    HINSTANCE opengl_instance = NULL;
    opengl_instance = (HINSTANCE)LoadLibraryA("opengl32.dll");
    if (opengl_instance) {
        FreeLibrary(opengl_instance);
        supported = true;
    }
#else
    #error Supported platform not found
#endif
    return supported;
}

const char* GraphicsDevice::_GetGraphicsAPIString(GraphicsAPI api)
{
    switch (api)
    {
        case GraphicsAPI::None:   return "None";
        case GraphicsAPI::OpenGL: return "OpenGL";
        case GraphicsAPI::Vulkan: return "Vulkan";
    };
    ZRN_CORE_ASSERT(false);
    return "Unknown API";
}

uint32_t GraphicsDevice::_GetShaderDataTypeSize(ShaderDataType type) {
    switch (type)
    {
        case ShaderDataType::None:      break;
        case ShaderDataType::Float:     return  1 * sizeof(float);
        case ShaderDataType::Float2:    return  2 * sizeof(float);
        case ShaderDataType::Float3:    return  3 * sizeof(float);
        case ShaderDataType::Float4:    return  4 * sizeof(float);
        case ShaderDataType::Mat3:      return  9 * sizeof(float);
        case ShaderDataType::Mat4:      return 16 * sizeof(float);
        case ShaderDataType::Int:       return  1 * sizeof(int);
        case ShaderDataType::Int2:      return  2 * sizeof(int);
        case ShaderDataType::Int3:      return  3 * sizeof(int);
        case ShaderDataType::Int4:      return  4 * sizeof(int);
        case ShaderDataType::Bool:      return  1 * sizeof(bool);
    }
    ZRN_CORE_ASSERT(false);
    return 0;
}

uint32_t GraphicsDevice::_GetShaderDataTypeComponentCount(ShaderDataType type) {
    switch (type)
    {
        case ShaderDataType::None:     return 0;
        case ShaderDataType::Float:    return 1;
        case ShaderDataType::Float2:   return 2;
        case ShaderDataType::Float3:   return 3;
        case ShaderDataType::Float4:   return 4;
        case ShaderDataType::Mat3:     return 9;
        case ShaderDataType::Mat4:     return 16;
        case ShaderDataType::Int:      return 1;
        case ShaderDataType::Int2:     return 2;
        case ShaderDataType::Int3:     return 3;
        case ShaderDataType::Int4:     return 4;
        case ShaderDataType::Bool:     return 1;
    }
    ZRN_CORE_ASSERT(false);
}

} // namespace zrn::Internal