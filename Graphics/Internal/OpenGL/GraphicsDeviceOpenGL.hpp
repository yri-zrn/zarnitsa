#pragma once

#include "Common.hpp"

#include "glad/glad.h"

namespace zrn::Internal
{

struct AttributeOpenGL
{
    GLuint     Index       = 0;
    GLint      Size        = 0;
    GLenum     Type        = 0;
    GLint      Offset      = 0;
    GLboolean  Normalized  = GL_FALSE;
};

struct VertexLayoutOpenGL
{
    GLuint           AttributeCount = 0;
    GLsizei          LayoutStride   = 0;
    AttributeOpenGL* Layout         = NULL;
};

struct VertexBufferOpenGL
{
    GLuint           VertexArray    = 0;
    GLuint           AttributeCount = 0;
    GLsizei          LayoutStride   = 0;
    AttributeOpenGL* Layout         = NULL;
};

struct TextureOpenGL
{
    GLenum Format;
    GLenum InternalFormat;
};

struct GraphicsDevice;

namespace GraphicsDeviceOpenGL
{
    bool Connect(GraphicsDevice* graphics_device);

    // void InputError(const char* description);

    bool InitDevice(GraphicsProc procedure);
    void TerminateDevice();
    // bool CreateRenderTarget(RenderTarget* target, RenderTargetConfig* config);
    // void ResizeRenderTarget(RenderTarget* target, int width, int height);
    // void DestroyRenderTarget(RenderTarget* target);
    // void SetRenderTarget(RenderTarget* target, int slot);

    void ClearBuffers();
    void Present();

    Status CreateVertexBuffer(Buffer* buffer, const VertexLayout* layout, const void* data, uint32_t size, bool mirror);
    Status CreateIndexBuffer(Buffer* buffer, const void* data, uint32_t size, bool mirror);
    Status CreateUniformBuffer(Buffer* buffer, const void* data, uint32_t size, bool mirror);
    void   DestroyBuffer(Buffer* buffer);
    void   BindVertexBuffer(Buffer* buffer, int stride, int offset);
    void   BindIndexBuffer(Buffer* buffer, int offset);
    void   BindUniformBuffer(Buffer* buffer, uint32_t slot);
    void   EditBufferDataRange(Buffer* buffer, const void* data, uint32_t size, uint32_t offset);
    void   EditBufferData(Buffer* buffer, const void* data);

    // TEMP: should be reworked to CreateShader(Shader*, ShaderType, const char* source);
    Status CreateVertexShader(Shader* shader, const char* source);
    Status CreateFragmentShader(Shader* shader, const char* source);
    
    Status CreateShaderBinary(Shader* shader, ShaderType type, const uint32_t* binary_source, size_t source_size);
    Status CreateVertexShaderBinary(Shader* shader, const uint32_t* binary_source, size_t source_size);
    Status CreateFragmentShaderBinary(Shader* shader, const uint32_t* binary_source, size_t source_size);
    void   DestroyShader(Shader* shader);

    Status CreateShaderProgram(ShaderProgram* program);
    void   DestroyShaderProgram(ShaderProgram* program);
    void   AttachShader(ShaderProgram* program, Shader* shader);
    Status LinkProgram(ShaderProgram* program);
    void   BindShaderProgram(ShaderProgram* program);

    Status CreateTexture(Texture* texture, const TextureSpecification* config, const void* data);
    void   BindTexture(Texture* texture, int slot);
    void   DestroyTexture(Texture* texture);

    // temp
    void   SubmitVertexBufferLayout(const Buffer* vertex_buffer);
    void   Draw(int numFaces, int index_offset, int vertex_offset);
    void   DrawDummy(uint32_t vertex_count = 3);

    void   SubmitVertexLayout(const VertexLayout* layout);

    Status CreateFramebuffer(Framebuffer* framebuffer, const FramebufferSpecification* config);
    void   DestroyFramebuffer(Framebuffer* framebuffer);
    void   BindFramebuffer(Framebuffer* framebuffer, uint32_t slot);
    void   ResizeFramebuffer(Framebuffer* framebuffer, uint32_t width, uint32_t height);

    void   SetViewport(int x, int y, uint32_t width, uint32_t height);
    void   SetClearColor(float r, float g, float b, float a);
    void   Clear();

    const char* _GetErrorString(GLenum error_code);
    void APIENTRY _GLDebugMessageCallback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar *msg,
                                          const void *data);

    GLenum _GetOpenGLShaderType(ShaderType type);
    GLenum _GetOpenGLFormatType(ShaderDataType type);
    GLenum _GetOpenGLBufferType(BufferType type);
    GLenum _GetOpenGLTextureFormat(TextureFormat format);
    GLenum _GetOpenGLInternalTextureFormat(TextureFormat format);
    GLenum _GetOpenGLDataType(TextureFormat format);
    
    
    inline GLuint _DummyVertexArray;
    extern GraphicsDevice* _GraphicsDevice;
};

} // namespace zrn::Internal