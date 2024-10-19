#pragma once

#include "ApplicationLayer.hpp"

#include "GraphicsDevice.hpp"
// #include "GraphicsContext.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "Scene/ViewportCamera.hpp"

#include "Scene/World.hpp"
#include "Scene/Components.hpp"

#include <iostream>

namespace zrn
{

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec2 TexCoords;
};

template<>
inline VertexLayout GetVertexLayout<Vertex>()
{
    return 
    {
        {
            ShaderDataType::Float3,
            ShaderDataType::Float3,
            ShaderDataType::Float3,
            ShaderDataType::Float2
        },
        4
    };
}

class DebugLayer;

class SandboxLayer : public ApplicationLayer<SandboxLayer>
{
public:
    SandboxLayer() = default;

    bool Init();
    void Terminate();
    void OnBegin();
    void OnUpdate(Timestep step);
    void OnEvent(Event event);
    void OnEnd();

    GraphicsDevice  Device;
    Renderer        Renderer;
    
    Shader*      BoxShader                  = nullptr;
    Shader*      GridShader                 = nullptr;
    Shader*      ScreenShader               = nullptr;
    Shader*      SolidColorShader           = nullptr;
    Shader*      OutlineShader              = nullptr;
    Shader*      DepthShader                = nullptr;

    Framebuffer  FramebufferOutline;
    Framebuffer  Depth;

    Texture      TextureAlbedo       = {};
    Texture      TextureRoughness    = {};
    Material     Material            = {};

    glm::vec4*   ClearColor = nullptr;

    World World;

private:
    DebugLayer* m_DebugLayer;
};



} // namespace zrn