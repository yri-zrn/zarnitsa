#pragma once

#include "ApplicationLayer.hpp"

#include "GraphicsDevice.hpp"
// #include "GraphicsContext.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
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

struct SceneData
{
    SceneData() = default;
    SceneData(glm::mat4 view_proj, glm::vec3 camera_pos)
        : ViewProjection(view_proj), CameraPosition(camera_pos) { }
    
    glm::mat4 ViewProjection;
    glm::vec3 CameraPosition;
    float _padding = 0;
};

template<>
inline uint32_t GetDataBinding<SceneData>() { return 1; };

struct ModelData
{
    ModelData() = default;
    ModelData(glm::mat4 model, glm::vec3 albedo, glm::vec3 rme, int mask)
        : Model(model), RME(rme), TextureMask(mask)
    {
        Albedo.x = albedo.x;
        Albedo.y = albedo.y;
        Albedo.z = albedo.z;
        Albedo.w = 1.0f;
    }
    
    glm::mat4 Model;
    glm::vec4 Albedo;
    glm::vec3 RME;
    int       TextureMask;
};

template<>
inline uint32_t GetDataBinding<ModelData>() { return 2; }

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

    GraphicsDevice  m_Device  = {};
    
    Shader*         BoxShader                  = nullptr;
    Shader*         GridShader                 = nullptr;
    Shader*         ScreenShaderMultisampled   = nullptr;
    Shader*         ScreenShader               = nullptr;
    Shader*         SolidColorShader           = nullptr;
    Shader*         OutlineShader              = nullptr;

    zrn::Framebuffer     FramebufferMultisampled;
    zrn::Framebuffer     Framebuffer;
    zrn::Framebuffer     FramebufferOutline;

    VertexBuffer    VertexBuffer        = {};
    
    UniformBuffer   ModelUniformBuffer  = {};
    UniformBuffer   SceneUniformBuffer  = {};

    IndexBuffer     IndexBuffer         = {};
    zrn::Texture    TextureAlbedo       = {};
    zrn::Texture    TextureRoughness    = {};
    Material        Material            = {};

    glm::vec4*      ClearColor = nullptr;

public:
    World World;

private:
    DebugLayer* m_DebugLayer;
};



} // namespace zrn