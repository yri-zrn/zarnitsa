#pragma once

#include "Internal/GraphicsDevice.hpp"

#include <memory>

namespace zrn
{

class GraphicsDevice;

class Texture;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class Shader;

class GraphicsContext
{
public:
    GraphicsContext() = default;

    bool ShouldMirror() { return true; }
    
    void SetDevice(GraphicsDevice* device) { m_Device = device; }
    GraphicsDevice* GetDevice();
    Internal::GraphicsDevice* GetInternalDevice();

    UniformBuffer* AttachedUniformBuffer;
    VertexBuffer*  AttachedVertexBuffer;
    IndexBuffer*   AttachedIndexBuffer;
    Texture*       AttachedTexture;
    Shader*        AttachedShader;

private:
    GraphicsDevice* m_Device = nullptr;
};

} // namespace zrn