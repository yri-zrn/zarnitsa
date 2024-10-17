#include "GraphicsDeviceOpenGL.hpp"

#include "GraphicsDevice.hpp"
#include "Internal.hpp"
#include "glad/glad.h"

namespace zrn::Internal
{

GraphicsDevice* GraphicsDeviceOpenGL::_GraphicsDevice = nullptr;

bool GraphicsDeviceOpenGL::Connect(GraphicsDevice* graphics_device)
{
    _GraphicsDevice = graphics_device;
    _GraphicsDevice->InitDevice                 = InitDevice;
    _GraphicsDevice->TerminateDevice            = TerminateDevice;
    // _GraphicsDevice->CreateRenderTarget     = CreateRenderTarget;
    // _GraphicsDevice->ResizeRenderTarget     = ResizeRenderTarget;
    // _GraphicsDevice->DestroyRenderTarget    = DestroyRenderTarget;
    // _GraphicsDevice->SetRenderTarget        = SetRenderTarget;
    _GraphicsDevice->ClearBuffers               = ClearBuffers;
    _GraphicsDevice->Present                    = Present;
    _GraphicsDevice->CreateVertexBuffer         = CreateVertexBuffer;
    _GraphicsDevice->CreateIndexBuffer          = CreateIndexBuffer;
    _GraphicsDevice->CreateUniformBuffer        = CreateUniformBuffer;
    _GraphicsDevice->DestroyBuffer              = DestroyBuffer;
    _GraphicsDevice->BindVertexBuffer           = BindVertexBuffer;
    _GraphicsDevice->BindIndexBuffer            = BindIndexBuffer;
    _GraphicsDevice->BindUniformBuffer          = BindUniformBuffer;
    _GraphicsDevice->EditBufferData             = EditBufferData;
    _GraphicsDevice->EditBufferDataRange        = EditBufferDataRange;
    _GraphicsDevice->CreateVertexShader         = CreateVertexShader;
    _GraphicsDevice->CreateFragmentShader       = CreateFragmentShader;
    _GraphicsDevice->CreateVertexShaderBinary   = CreateVertexShaderBinary;
    _GraphicsDevice->CreateFragmentShaderBinary = CreateFragmentShaderBinary;
    _GraphicsDevice->DestroyShader              = DestroyShader;
    _GraphicsDevice->CreateShaderProgram        = CreateShaderProgram;
    _GraphicsDevice->DestroyShaderProgram       = DestroyShaderProgram;
    _GraphicsDevice->AttachShader               = AttachShader;
    _GraphicsDevice->LinkProgram                = LinkProgram;
    _GraphicsDevice->BindShaderProgram          = BindShaderProgram;
    _GraphicsDevice->CreateTexture              = CreateTexture;
    _GraphicsDevice->BindTexture                = BindTexture;
    _GraphicsDevice->DestroyTexture             = DestroyTexture;
    _GraphicsDevice->CreateFramebuffer          = CreateFramebuffer;
    _GraphicsDevice->DestroyFramebuffer         = DestroyFramebuffer;
    _GraphicsDevice->BindFramebuffer            = BindFramebuffer;
    _GraphicsDevice->ResizeFramebuffer          = ResizeFramebuffer;
    _GraphicsDevice->SetViewport                = SetViewport;
    _GraphicsDevice->SetClearColor              = SetClearColor;
    _GraphicsDevice->Clear                      = Clear;
    _GraphicsDevice->SubmitVertexBufferLayout   = SubmitVertexBufferLayout; // TEMP
    _GraphicsDevice->Draw                       = Draw;
    _GraphicsDevice->DrawDummy                 = DrawDummy;
    _GraphicsDevice->SubmitVertexLayout         = SubmitVertexLayout;
    return true;
}

// void GraphicsDeviceOpenGL::InputError(const char* description)
// {
//     GLenum error = glGetError();
//     // _GraphicsDevice->InputError("%s: %s", description, _GetErrorString(error));
// }

bool GraphicsDeviceOpenGL::InitDevice(GraphicsProc procedure)
{
    if (procedure)
        gladLoadGLLoader((GLADloadproc)procedure);
    
    if (!gladLoadGL())
        return false;

    ZRN_ON_DEBUG(
        int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
            glDebugMessageCallback(_GLDebugMessageCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    )

    glGenVertexArrays(1, &_DummyVertexArray);

    return true;
}

void GraphicsDeviceOpenGL::TerminateDevice()
{
    glDeleteVertexArrays(1, &_DummyVertexArray);
    // ZRN_NOT_IMPLEMENTED();
}

// bool GraphicsDeviceOpenGL::CreateRenderTarget(RenderTarget* target, RenderTargetConfig* config)
// {
//     ZRN_NOT_IMPLEMENTED();
// }

// void GraphicsDeviceOpenGL::ResizeRenderTarget(RenderTarget* target, int width, int height)
// {
//     ZRN_NOT_IMPLEMENTED();
// }

// void GraphicsDeviceOpenGL::DestroyRenderTarget(RenderTarget* target)
// {
//     ZRN_NOT_IMPLEMENTED();
// }

// void GraphicsDeviceOpenGL::SetRenderTarget(RenderTarget* target, int slot)
// {
//     ZRN_NOT_IMPLEMENTED();
// }

void GraphicsDeviceOpenGL::ClearBuffers()
{
    ZRN_NOT_IMPLEMENTED();
}

void GraphicsDeviceOpenGL::Present()
{
    ZRN_NOT_IMPLEMENTED();
}

Status GraphicsDeviceOpenGL::CreateVertexBuffer(Buffer* buffer, const VertexLayout* layout, const void* data, uint32_t size, bool mirror)
{
    // Legacy OpenGL API
    // glGenBuffers(1, (GLuint*)&buffer->ID);
    // glBindBuffer(GL_ARRAY_BUFFER, buffer->ID);
    // glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    
    glCreateBuffers(1, (GLuint*)&buffer->ID);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->ID);
    glNamedBufferStorage(buffer->ID, size, data, 0);

    if (!buffer->ID)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL vertex buffer");
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    VertexBufferOpenGL* handle = (VertexBufferOpenGL*)calloc(1, sizeof(VertexBufferOpenGL));
    glGenVertexArrays(1, &handle->VertexArray);
    glBindVertexArray(handle->VertexArray);

    if (!handle->VertexArray)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL vertex array: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }
    
    handle->AttributeCount = layout->AttributeCount;
    handle->Layout = (AttributeOpenGL*)calloc(layout->AttributeCount, sizeof(AttributeOpenGL));
    
    uint32_t offset = 0;
    for (size_t i = 0; i < layout->AttributeCount; ++i)
    {
        handle->Layout[i].Index      = i;
        handle->Layout[i].Normalized = layout->Attributes[i].Normalized;
        handle->Layout[i].Size       = _GraphicsDevice->_GetShaderDataTypeComponentCount(layout->Attributes[i].Type);
        handle->Layout[i].Offset     = offset;
        handle->Layout[i].Type       = _GetOpenGLFormatType(layout->Attributes[i].Type);

        offset += _GraphicsDevice->_GetShaderDataTypeSize(layout->Attributes[i].Type);
    }
    handle->LayoutStride = offset;

    buffer->API    = GraphicsAPI::OpenGL;
    buffer->Handle = (void*)handle;
    buffer->Size   = size;
    buffer->Type   = BufferType::Vertex;

    if (mirror)
    {
        buffer->Data = (char*)calloc(size, sizeof(char));
        ZRN_CORE_ASSERT(data);
        memcpy(buffer->Data, data, size);
    }

    return ErrorType::NoError;
}

Status GraphicsDeviceOpenGL::CreateIndexBuffer(Buffer* buffer, const void* data, uint32_t size, bool mirror)
{
    // Legacy API
    // glGenBuffers(1, (GLuint*)&buffer->ID);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ID);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)data, GL_STATIC_DRAW);

    glCreateBuffers(1, (GLuint*)&buffer->ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ID);
    glNamedBufferStorage(buffer->ID, size, data, 0);

    if (!buffer->ID)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL index buffer: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    buffer->API    = GraphicsAPI::OpenGL;
    buffer->Size   = size;
    buffer->Type   = BufferType::Index;
    
    if (mirror)
    {
        buffer->Data = (char*)calloc(size, sizeof(char));
        ZRN_CORE_ASSERT(data);
        memcpy(buffer->Data, data, size);
    }

    return ErrorType::NoError;
}

Status GraphicsDeviceOpenGL::CreateUniformBuffer(Buffer* buffer, const void* data, uint32_t size, bool mirror)
{
    glGenBuffers(1, (GLuint*)&buffer->ID);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->ID);
    glBufferData(GL_UNIFORM_BUFFER, size, (void*)data, GL_DYNAMIC_DRAW);

    if (!buffer->ID)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL uniform buffer");
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    buffer->API    = GraphicsAPI::OpenGL;
    buffer->Size   = size;
    buffer->Type   = BufferType::Uniform;

    if (mirror && data)
    {
        buffer->Data = (char*)calloc(size, sizeof(char));
        memcpy(buffer->Data, data, size);
    }
    
    return ErrorType::NoError;
}

void GraphicsDeviceOpenGL::DestroyBuffer(Buffer* buffer)
{
    if (!buffer)
        return;
    ZRN_CORE_ASSERT(buffer->API == _GraphicsDevice->_API);
    
    glDeleteBuffers(1, (GLuint*)&buffer->ID);

    if (buffer->Type == BufferType::Vertex)
        glDeleteVertexArrays(1, &((VertexBufferOpenGL*)buffer->Handle)->VertexArray);
}

void GraphicsDeviceOpenGL::BindVertexBuffer(Buffer* buffer, int stride, int offset)
{
    ZRN_CORE_ASSERT(buffer->API == GraphicsAPI::OpenGL);
    ZRN_CORE_ASSERT(buffer->Type == BufferType::Vertex);

    if (buffer)
    {
        glBindVertexArray(((VertexBufferOpenGL*)buffer->Handle)->VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->ID);
    }
    else
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GraphicsDeviceOpenGL::BindIndexBuffer(Buffer* buffer, int offset)
{
    ZRN_CORE_ASSERT(buffer->API == _GraphicsDevice->_API);
    ZRN_CORE_ASSERT(buffer->Type == BufferType::Index);

    if (buffer)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ID);
    }
    else
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void GraphicsDeviceOpenGL::BindUniformBuffer(Buffer* buffer, uint32_t slot)
{
    ZRN_CORE_ASSERT(buffer->API == _GraphicsDevice->_API);
    ZRN_CORE_ASSERT(buffer->Type == BufferType::Uniform);

    if (buffer)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, slot, buffer->ID, 0, buffer->Size);
    }
    else
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void GraphicsDeviceOpenGL::EditBufferDataRange(Buffer* buffer, const void* data, uint32_t size, uint32_t offset)
{
    ZRN_CORE_ASSERT(buffer->API == _GraphicsDevice->_API);

    GLenum buffer_type = _GetOpenGLBufferType(buffer->Type);
    glBindBuffer(buffer_type, buffer->ID);
    glBufferSubData(buffer_type, offset, size, data);
}

void GraphicsDeviceOpenGL::EditBufferData(Buffer* buffer, const void* data)
{
    ZRN_CORE_ASSERT(buffer->API == _GraphicsDevice->_API);

    GLenum buffer_type = _GetOpenGLBufferType(buffer->Type);
    glBindBuffer(buffer_type, buffer->ID);
    glBufferSubData(buffer_type, 0, buffer->Size, data);
}


Status GraphicsDeviceOpenGL::CreateVertexShader(Shader* shader, const char* source)
{
    GLuint id = glCreateShader(GL_VERTEX_SHADER);
    if (!id)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL vertex shader: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        GLchar message[1024];
        GLsizei length; 
        glGetShaderInfoLog(id, 1024, &length, message);
        ZRN_CORE_ERROR("Failed to compile OpenGL vertex shader: {0}", message);
        return { ErrorType::ShaderCompilation, message };
    }

    shader->API  = GraphicsAPI::OpenGL;
    shader->Type = ShaderType::Vertex;
    shader->ID   = id;

    return ErrorType::NoError;
}

Status GraphicsDeviceOpenGL::CreateFragmentShader(Shader* shader, const char* source)
{
    GLuint id = glCreateShader(GL_FRAGMENT_SHADER);
    if (!id)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL fragment shader: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);
    
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        GLchar message[1024];
        GLsizei length; 
        glGetShaderInfoLog(id, 1024, &length, message);
        ZRN_CORE_ERROR("Failed to compile OpenGL fragment shader: {0}", message);
        return { ErrorType::ResourceCreation, message };
    }

    shader->API  = GraphicsAPI::OpenGL;
    shader->Type = ShaderType::Fragment;
    shader->ID   = id;

    return ErrorType::NoError;
}

Status GraphicsDeviceOpenGL::CreateShaderBinary(Shader* shader, ShaderType type, const uint32_t* binary_source, size_t source_size)
{
    GLuint id = glCreateShader(_GetOpenGLShaderType(type));
    if (!id)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL binary shader: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, binary_source, source_size);
    glSpecializeShader(id, "main", 0, nullptr, nullptr);

    shader->API  = GraphicsAPI::OpenGL;
    shader->Type = type;
    shader->ID   = id;
    
    return ErrorType::NoError;
}

Status GraphicsDeviceOpenGL::CreateVertexShaderBinary(Shader* shader, const uint32_t* binary_source, size_t source_size)
{
    return CreateShaderBinary(shader, ShaderType::Vertex, binary_source, source_size);
}

Status GraphicsDeviceOpenGL::CreateFragmentShaderBinary(Shader* shader, const uint32_t* binary_source, size_t source_size)
{
    return CreateShaderBinary(shader, ShaderType::Fragment, binary_source, source_size);
}

void GraphicsDeviceOpenGL::DestroyShader(Shader* shader)
{
    if (!shader)
        return;
    
    glDeleteShader(shader->ID);
}

Status GraphicsDeviceOpenGL::CreateShaderProgram(ShaderProgram* program)
{
    GLuint id = glCreateProgram(); 
    if (!id)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL shader program: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    program->API = GraphicsAPI::OpenGL;
    program->ID  = id;
    memset(program->ShaderSlots, (int)ShaderType::None, sizeof(program->ShaderSlots));

    return ErrorType::NoError;
}

void GraphicsDeviceOpenGL::DestroyShaderProgram(ShaderProgram* program)
{
    if (!program)
        return;
    
    for (size_t i = 0; i < (size_t)ShaderType::Last; ++i)
        glDetachShader(program->ID, program->ShaderSlots[i]->ID);

    glDeleteShader(program->ID);
}

void GraphicsDeviceOpenGL::AttachShader(ShaderProgram* program, Shader* shader)
{
    glAttachShader(program->ID, shader->ID);

    program->ShaderSlots[(size_t)shader->Type] = shader;
}

Status GraphicsDeviceOpenGL::LinkProgram(ShaderProgram* program)
{
    glLinkProgram(program->ID);
    glValidateProgram(program->ID);

    GLint success;
    glGetProgramiv(program->ID, GL_LINK_STATUS, &success);

    if (success != GL_TRUE)
    {
        GLchar message[GL_INFO_LOG_LENGTH];
        GLsizei length; 
        glGetProgramInfoLog(program->ID, GL_INFO_LOG_LENGTH, &length, message);
        
        ZRN_CORE_ERROR("Failed to link OpenGL shader program: {0}", message);
        return { ErrorType::ShaderLinking, message };
    }

    return ErrorType::NoError;
}

void GraphicsDeviceOpenGL::BindShaderProgram(ShaderProgram* program)
{
    if (program)
    {
        glUseProgram(program->ID);
    }
    else
    {
        glUseProgram(0);
    }
}

Status GraphicsDeviceOpenGL::CreateTexture(Texture* texture, const TextureSpecification* config, const void* data)
{
    GLuint handle;
    glGenTextures(1, &handle);

    if (!handle)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL texture: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    glBindTexture(GL_TEXTURE_2D, handle);

    // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // glPixelStorei(GL_PACK_ALIGNMENT, 4);
    
    GLenum format          = _GetOpenGLTextureFormat(config->Format);
    GLenum internal_format = _GetOpenGLInternalTextureFormat(config->Format);

    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    glTextureStorage2D(handle, 1, internal_format, config->Width, config->Height);

    // TEMP: Should add ability to change filters, mipmaps, format
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (config->Format == TextureFormat::RGB8)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTextureSubImage2D(handle, 0, 0, 0, config->Width, config->Height, format, GL_UNSIGNED_BYTE, data);

    TextureOpenGL* texture_opengl = (TextureOpenGL*)calloc(1, sizeof(TextureOpenGL));
    texture_opengl->Format         = format;
    texture_opengl->InternalFormat = internal_format;

    // glTexImage2D(GL_TEXTURE_2D, 0, format, config->Width, config->Height, 0, internal_format, GL_UNSIGNED_BYTE, data);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    texture->API    = GraphicsAPI::OpenGL;
    texture->Handle = texture_opengl;
    texture->ID     = handle;
    texture->Width  = config->Width;
    texture->Height = config->Height;
    texture->Format = config->Format;

    return ErrorType::NoError;
}

void GraphicsDeviceOpenGL::BindTexture(Texture* texture, int slot)
{
    if (texture)
    {
        glBindTextureUnit(slot, texture->ID);
    }
    else
    {
        glBindTextureUnit(slot, 0);
    }
}

void GraphicsDeviceOpenGL::DestroyTexture(Texture* texture)
{
    if (!texture)
        return;
    
    glDeleteTextures(1, (const GLuint*)&texture->ID);
}

// TEMP
void GraphicsDeviceOpenGL::SubmitVertexBufferLayout(const Buffer* vertex_buffer)
{
    ZRN_CORE_ASSERT(vertex_buffer->Type == BufferType::Vertex);
    ZRN_CORE_ASSERT(vertex_buffer->API == GraphicsAPI::OpenGL);
    
    VertexBufferOpenGL* vertex_buffer_opengl = (VertexBufferOpenGL*)vertex_buffer->Handle;

    for (size_t i = 0; i < vertex_buffer_opengl->AttributeCount; ++i)
    {
        AttributeOpenGL* layout = vertex_buffer_opengl->Layout + i;
        glEnableVertexAttribArray(layout->Index);
        switch (layout->Type)
        {
        case GL_FLOAT:
            glVertexAttribPointer( layout->Index, layout->Size, GL_FLOAT, layout->Normalized, vertex_buffer_opengl->LayoutStride, (void*)(uint64_t)layout->Offset);
            break;
        case GL_INT:
        case GL_BOOL:
            glVertexAttribPointer( layout->Index, layout->Size, GL_FLOAT, layout->Normalized, vertex_buffer_opengl->LayoutStride, (void*)(uint64_t)layout->Offset);
            break;
        }
    }
}

void GraphicsDeviceOpenGL::Draw(int numFaces, int indexOffset, int vertexOffset)
{
    // GL_UNSIGNED_SHORT
    glDrawElementsBaseVertex(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)(uint64_t)(indexOffset * 2), vertexOffset);
}

void GraphicsDeviceOpenGL::DrawDummy(uint32_t vertex_count)
{
    glBindVertexArray(_DummyVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void GraphicsDeviceOpenGL::SubmitVertexLayout(const VertexLayout* layout)
{
    uint32_t offset = 0;
    for (size_t i = 0; i < layout->AttributeCount; ++i)
    {
        offset += _GraphicsDevice->_GetShaderDataTypeSize(layout->Attributes[i].Type);
    }
    GLsizei LayoutStride = offset;

    offset = 0;
    for (size_t i = 0; i < layout->AttributeCount; ++i)
    {
        GLuint     Index      = i;
        GLint      Size       = _GraphicsDevice->_GetShaderDataTypeComponentCount(layout->Attributes[i].Type);
        GLenum     Type       = _GetOpenGLFormatType(layout->Attributes[i].Type);
        GLint      Offset     = offset;
        GLboolean  Normalized = layout->Attributes[i].Normalized;

        glEnableVertexAttribArray(Index);
        switch (Type)
        {
        case GL_FLOAT:
            glVertexAttribPointer(Index, Size, GL_FLOAT, Normalized, LayoutStride, (void*)(uint64_t)Offset);
            break;
        case GL_INT:
        case GL_BOOL:
            glVertexAttribPointer(Index, Size, GL_FLOAT, Normalized, LayoutStride, (void*)(uint64_t)Offset);
            break;
        }
        offset += _GraphicsDevice->_GetShaderDataTypeSize(layout->Attributes[i].Type);
    }
}

Status GraphicsDeviceOpenGL::CreateFramebuffer(Framebuffer* framebuffer, const FramebufferSpecification* config)
{
    memset(framebuffer->ColorAttachments, 0, sizeof(framebuffer->ColorAttachments));
    framebuffer->DepthAttachment = 0;

    GLuint handle;
    glCreateFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    if (!handle)
    {
        ZRN_CORE_ERROR("Failed to create OpenGL framebuffer: {0}", _GetErrorString(glGetError()));
        return { ErrorType::ResourceCreation, _GetErrorString(glGetError()) };
    }

    bool multisample = config->Samples > 1;

    GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glCreateTextures(target, config->ColorAttachmentsCount, framebuffer->ColorAttachments);

    for (size_t i = 0; i < config->ColorAttachmentsCount; ++i)
    {
        glBindTexture(target, framebuffer->ColorAttachments[i]);
        
        GLenum format = _GetOpenGLTextureFormat(config->Attachments[i]);
        GLenum internal_format = _GetOpenGLInternalTextureFormat(config->Attachments[i]);

        if (multisample)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, config->Samples, internal_format, config->Width, config->Height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, config->Width, config->Height, 0, format, _GetOpenGLDataType(config->Attachments[i]), NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, framebuffer->ColorAttachments[i], 0);
    }

    if (config->DepthAttachment != TextureFormat::None)
    {
        glCreateTextures(target, 1, &framebuffer->DepthAttachment);
        glBindTexture(target, framebuffer->DepthAttachment);
        
        GLenum format = GL_DEPTH24_STENCIL8;

        if (multisample)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, config->Samples, format, config->Width, config->Height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, config->Width, config->Height);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, framebuffer->DepthAttachment, 0);
    }

    if (config->ColorAttachmentsCount > 1)
    {
        GLenum buffers[16] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5,
            GL_COLOR_ATTACHMENT6,
            GL_COLOR_ATTACHMENT7,
            GL_COLOR_ATTACHMENT8,
            GL_COLOR_ATTACHMENT9,
            GL_COLOR_ATTACHMENT10,
            GL_COLOR_ATTACHMENT11,
            GL_COLOR_ATTACHMENT12,
            GL_COLOR_ATTACHMENT13,
            GL_COLOR_ATTACHMENT14,
            GL_COLOR_ATTACHMENT15
        };
        glDrawBuffers(config->ColorAttachmentsCount, buffers);
    }
    else if (config->ColorAttachmentsCount == 0)
    {
        glDrawBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ZRN_CORE_ERROR("Created framebuffer is incomplete, abort: {0}", _GetErrorString(glGetError()));
        return { ErrorType::FramebufferIncomplete, _GetErrorString(glGetError()) };
    }

    framebuffer->ID                    = handle;
    framebuffer->Width                 = config->Width;
    framebuffer->Height                = config->Height;
    framebuffer->ColorAttachmentsCount = config->ColorAttachmentsCount;
    // framebuffer->ColorAttachmentFormats = 
    // memcpy_s(framebuffer->ColorAttachmentFormats, sizeof(framebuffer->ColorAttachmentFormats) / sizeof(TextureFormat), config->Attachments, config->ColorAttachmentsCount);
    memcpy(framebuffer->ColorAttachmentFormats, config->Attachments, sizeof(config->Attachments));
    framebuffer->DepthAttachmentFormat = config->DepthAttachment;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ErrorType::NoError;
}

void GraphicsDeviceOpenGL::DestroyFramebuffer(Framebuffer* framebuffer)
{

}

void GraphicsDeviceOpenGL::BindFramebuffer(Framebuffer* framebuffer, uint32_t slot)
{
    if (framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->ID);
        glViewport(0, 0, framebuffer->Width, framebuffer->Height);

        for (size_t slot = 0; slot < framebuffer->ColorAttachmentsCount; ++slot)
            glBindTextureUnit(slot, framebuffer->ColorAttachments[slot]);
    }
    else
    {
        // TEMP: add Unbind function for framebuffer
        // for (size_t slot = 0; slot < framebuffer->ColorAttachmentsCount; ++slot)
        //     glBindTextureUnit(slot, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GraphicsDeviceOpenGL::ResizeFramebuffer(Framebuffer* framebuffer, uint32_t width, uint32_t height)
{
    ZRN_NOT_IMPLEMENTED();
}

void GraphicsDeviceOpenGL::SetViewport(int x, int y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void GraphicsDeviceOpenGL::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void GraphicsDeviceOpenGL::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}


const char* GraphicsDeviceOpenGL::_GetErrorString(GLenum error_code)
{
    switch (error_code)
    {
    case GL_NO_ERROR:                      return "No error";
    case GL_INVALID_ENUM:                  return "Invalid enum";
    case GL_INVALID_VALUE:                 return "Invalid value";
    case GL_INVALID_OPERATION:             return "Invalid operation";
    case GL_STACK_OVERFLOW:                return "Stack overflow";
    case GL_STACK_UNDERFLOW:               return "Stack underflow";
    case GL_OUT_OF_MEMORY:                 return "Out of memory";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
    }
    return "Unknown error";
}

void GraphicsDeviceOpenGL::_GLDebugMessageCallback(GLenum source,
                                                   GLenum type,
                                                   GLuint id,
                                                   GLenum severity,
                                                   GLsizei length,
                                                   const GLchar *msg,
                                                   const void *data)
{
    const char* _source;
    const char* _type;
    const char* _severity;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:               _source = "API";              break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     _source = "Window System";    break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   _source = "Shader Compiler";  break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       _source = "Third Party";      break;
        case GL_DEBUG_SOURCE_APPLICATION:       _source = "Application";      break;
        default: _source = "UNKNOWN"; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:                  _type = "Error";                 break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:    _type = "Deprecated behavior";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:     _type = "Undefined behavior";    break;
        case GL_DEBUG_TYPE_PORTABILITY:            _type = "Portability";           break;
        case GL_DEBUG_TYPE_PERFORMANCE:            _type = "Perfomance";           break;
        case GL_DEBUG_TYPE_OTHER:                  _type = "Other";                 break;
        case GL_DEBUG_TYPE_MARKER:                 _type = "Marker";                break;
        default: _type = "UNKNOWN"; break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            ZRN_CORE_ERROR("OpenGL [{0}, {1}]: {2}", _source, _type, msg);
        case GL_DEBUG_SEVERITY_MEDIUM:
            ZRN_CORE_WARN("OpenGL [{0}, {1}]: {2}", _source, _type, msg);
        case GL_DEBUG_SEVERITY_LOW:
            ZRN_CORE_WARN("OpenGL [{0}, {1}]: {2}", _source, _type, msg);
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            ZRN_CORE_INFO("OpenGL [{0}, {1}]: {2}", _source, _type, msg);
        default:
            ZRN_CORE_INFO("OpenGL [{0}, {1}]: {2}", _source, _type, msg);
    }
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLShaderType(ShaderType type)
{
    switch (type)
    {
        case ShaderType::None:      break;
        case ShaderType::Compute:   return GL_COMPUTE_SHADER;
        case ShaderType::Vertex:    return GL_VERTEX_SHADER;
        case ShaderType::Fragment:  return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry:  return GL_GEOMETRY_SHADER;
        default: break;
    };

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLFormatType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::None:      break;
        case ShaderDataType::Float:     return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT;
        case ShaderDataType::Float3:    return GL_FLOAT;
        case ShaderDataType::Float4:    return GL_FLOAT;
        case ShaderDataType::Mat3:      return GL_FLOAT;
        case ShaderDataType::Mat4:      return GL_FLOAT;
        case ShaderDataType::Int:       return GL_INT;
        case ShaderDataType::Int2:      return GL_INT;
        case ShaderDataType::Int3:      return GL_INT;
        case ShaderDataType::Int4:      return GL_INT;
        case ShaderDataType::Bool:      return GL_BOOL;
        default: break;
    };

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLBufferType(BufferType type)
{
    switch (type)
    {
        case BufferType::Vertex:  return GL_ARRAY_BUFFER;
        case BufferType::Index:   return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Uniform: return GL_UNIFORM_BUFFER;
        default: break;
    };

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLTextureFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB8:       return GL_RGB;
        case TextureFormat::RGBA8:      return GL_RGBA;
        case TextureFormat::RGB16F:     return GL_RGB;
        case TextureFormat::RedInteger: return GL_RED_INTEGER;
        default: break;
    }

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLInternalTextureFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB8:       return GL_RGB8;
        case TextureFormat::RGBA8:      return GL_RGBA8;
        case TextureFormat::RGB16F:     return GL_RGB16F;
        case TextureFormat::RedInteger: return GL_R32I;
        default: break;
    }

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

GLenum GraphicsDeviceOpenGL::_GetOpenGLDataType(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB8:       return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA8:      return GL_UNSIGNED_BYTE;
        case TextureFormat::RGB16F:     return GL_FLOAT;
        case TextureFormat::RedInteger: return GL_UNSIGNED_BYTE;
        default: break;
    }

    ZRN_NOT_IMPLEMENTED();
    return GL_FALSE;
}

} // namespace zrn::Internal
