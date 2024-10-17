#pragma once

#include "Common.hpp"

namespace zrn::Internal
{

struct Buffer
{
    Buffer() = default;

    GraphicsAPI API     = GraphicsAPI::None;
    void*       Handle  = nullptr;
    DeviceID    ID      = 0;
    BufferType  Type    = BufferType::None;
    size_t      Size    = 0;
    char*       Data    = nullptr;
};

struct Shader
{
    Shader() = default;

    GraphicsAPI API     = GraphicsAPI::None;
    void*       Handle  = nullptr;
    DeviceID    ID      = 0;
    ShaderType  Type    = ShaderType::None;
};

struct ShaderProgram
{
    ShaderProgram() = default;

    GraphicsAPI API     = GraphicsAPI::None;
    void*       Handle  = nullptr;
    DeviceID    ID      = 0;
    Shader*     ShaderSlots[(size_t)ShaderType::Last];
};

struct Texture
{
    Texture() = default;

    GraphicsAPI   API    = GraphicsAPI::None;
    void*         Handle = nullptr;
    DeviceID      ID     = 0;
    uint32_t      Width  = 0;
    uint32_t      Height = 0;
    TextureFormat Format = TextureFormat::None;
};

struct Framebuffer
{
    Framebuffer() = default;
    
    GraphicsAPI   API    = GraphicsAPI::None;
    void*         Handle = nullptr;
    DeviceID      ID     = 0;
    uint32_t      Width  = 0;
    uint32_t      Height = 0;
    uint32_t      ColorAttachmentsCount = 0;
    TextureFormat ColorAttachmentFormats[16];
    DeviceID      ColorAttachments[16];
    TextureFormat DepthAttachmentFormat;
    DeviceID      DepthAttachment;

};

} // namespace zrn::Internal