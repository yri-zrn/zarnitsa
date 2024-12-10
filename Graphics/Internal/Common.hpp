#pragma once

// TEMP: i guess?
#include "../../Common/Common.hpp"

namespace zrn
{

typedef void (*GraphicsProc)(const char*);

typedef uint32_t DeviceID;

enum class GraphicsAPI
{
    None = -1,
    OpenGL,
    Vulkan
};

enum class BufferType
{
    None      = -1,
    Vertex    = 0,
    Index     = 1,
    Uniform   = 2
};

enum class ShaderType
{
    None      = -1,
    Vertex    = 0,
    Geometry  = 1,
    Compute   = 2,
    Fragment  = 3,
    Last      = Fragment
};

enum class ShaderDataType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

enum class TextureFormat
{
    None = 0,
    RGB8,
    RGBA8,
    RGB16F,
    RGBA16F,
    Red,
    RedInteger,
    Depth24Stencil8
};

struct TextureSpecification
{
    TextureSpecification() = default;

    uint32_t      Width  = 0;
    uint32_t      Height = 0;
    TextureFormat Format = TextureFormat::None;
    // filtering
    // wrapping
    // mipmapping?
};

struct FramebufferSpecification
{
    FramebufferSpecification() = default;

    uint32_t      Width              = 0;
    uint32_t      Height             = 0;
    uint32_t      Samples            = 1;
    uint32_t      ColorAttachmentsCount   = 0;
    TextureFormat Attachments[16];
    TextureFormat DepthAttachment;
};

struct Attribute
{
    Attribute() = default;

    Attribute(ShaderDataType type) : Type(type) { }
    ShaderDataType  Type       = ShaderDataType::None;
    bool            Normalized = false;
};

struct VertexLayout
{
    VertexLayout() = default;
    
    Attribute Attributes[16];
    uint32_t AttributeCount  = 0;
};

struct MeshSpecification
{
    void*        VertexData;
    size_t       VertexDataSize;
    uint32_t*    IndexData;
    size_t       IndexDataSize;
    VertexLayout VertexBufferLayout;
};

namespace Internal
{

struct Buffer;
struct Shader;

struct ShaderProgram;

struct VertexDataType;
// struct Attribute;
// struct BufferLayout;
struct PushConstant;

struct Texture;
struct Framebuffer;

} // namespace Internal

} // namespace zrn