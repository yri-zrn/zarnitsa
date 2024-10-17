#pragma once

#include <stdint.h>

#include "Internal.hpp"
#include "OpenGL/GraphicsDeviceOpenGL.hpp"

namespace zrn::Internal
{

struct GraphicsDevice
{
public:
    GraphicsDevice() = default;
    
    bool Init(GraphicsAPI desired_api = GraphicsAPI::OpenGL, GraphicsProc proc = nullptr);

    void Terminate();
    
    bool Connect(GraphicsAPI desired_api);

    // void          InputError(const char* format, ...);

    bool (*InitDevice)(GraphicsProc procedure) = nullptr;
    void (*TerminateDevice)() = nullptr;
    // bool (*CreateRenderTarget)(RenderTarget* target, RenderTargetConfig* config);
    // void (*ResizeRenderTarget)(RenderTarget* target, int width, int height);
    // void (*DestroyRenderTarget)(RenderTarget* target);
    // void (*SetRenderTarget)(RenderTarget* target, int slot);

    void (*ClearBuffers)() = nullptr;
    void (*Present)() = nullptr;

    Status (*CreateVertexBuffer)(Buffer* buffer, const VertexLayout* layout, const void* data, uint32_t size, bool mirror) = nullptr;
    Status (*CreateIndexBuffer)(Buffer* buffer, const void* data, uint32_t size, bool mirror) = nullptr;
    Status (*CreateUniformBuffer)(Buffer* buffer, const void* data, uint32_t size, bool mirror) = nullptr;
    void (*DestroyBuffer)(Buffer* buffer) = nullptr;
    void (*BindVertexBuffer)(Buffer* buffer, int stride, int offset) = nullptr;
    void (*BindIndexBuffer)(Buffer* buffer, int offset) = nullptr;
    void (*BindUniformBuffer)(Buffer* buffer, uint32_t slot) = nullptr;
    void (*EditBufferDataRange)(Buffer* buffer, const void* data, uint32_t size, uint32_t offset) = nullptr;
    void (*EditBufferData)(Buffer* buffer, const void* data) = nullptr;

    Status (*CreateVertexShader)(Shader* shader, const char* source) = nullptr;
    Status (*CreateFragmentShader)(Shader* shader, const char* source) = nullptr;
    Status (*CreateVertexShaderBinary)(Shader* shader, const uint32_t* binary_source, size_t source_size) = nullptr;
    Status (*CreateFragmentShaderBinary)(Shader* shader, const uint32_t* binary_source, size_t source_size) = nullptr;
    void (*DestroyShader)(Shader* shader) = nullptr;

    Status (*CreateShaderProgram)(ShaderProgram* program) = nullptr;
    void (*DestroyShaderProgram)(ShaderProgram* program) = nullptr;
    void (*AttachShader)(ShaderProgram* program, Shader* shader) = nullptr;
    Status (*LinkProgram)(ShaderProgram* program) = nullptr;
    void (*BindShaderProgram)(ShaderProgram* program) = nullptr;

    Status (*CreateTexture)(Texture* texture, const TextureSpecification* config, const void* data) = nullptr;
    void (*BindTexture)(Texture* texture, int slot) = nullptr;
    void (*DestroyTexture)(Texture* texture) = nullptr;

    Status (*CreateFramebuffer)(Framebuffer* framebuffer, const FramebufferSpecification* config) = nullptr;
    void (*DestroyFramebuffer)(Framebuffer* framebuffer) = nullptr;
    void (*BindFramebuffer)(Framebuffer* framebuffer, uint32_t slot) = nullptr;
    void (*ResizeFramebuffer)(Framebuffer* framebuffer, uint32_t width, uint32_t height) = nullptr;

    void (*SetViewport)(int x, int y, uint32_t width, uint32_t height);
    void (*SetClearColor)(float r, float g, float b, float a);
    void (*Clear)();

    // TEMP
    void (*SubmitVertexBufferLayout)(const Buffer* vertex_buffer) = nullptr;
    void (*Draw)(int faces_count, int index_offset, int vertex_offset) = nullptr;
    void (*DrawDummy)(uint32_t vertex_count) = nullptr;

    void (*SubmitVertexLayout)(const VertexLayout* layout) = nullptr;

    GraphicsAPI   _API              = GraphicsAPI::None;
    bool          _Initialized      = false;
    bool          _OpenGLSupported  = false;
    bool          _VulkanSupported  = false;

    bool        _CheckOpenGLSupport();
    const char* _GetGraphicsAPIString(GraphicsAPI api);
    uint32_t    _GetShaderDataTypeSize(ShaderDataType type);
    uint32_t    _GetShaderDataTypeComponentCount(ShaderDataType type);
};

} // namespace zrn::Internal