#pragma once

#include "Common.hpp"
#include "Buffer.hpp"
#include "Framebuffer.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "GraphicsDevice.hpp"

namespace zrn
{

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

// struct ModelData
// {
//     ModelData() = default;
//     ModelData(glm::mat4 model, glm::vec3 albedo, glm::vec3 rme, int mask)
//         : Model(model), RME(rme), TextureMask(mask)
//     {
//         Albedo.x = albedo.x;
//         Albedo.y = albedo.y;
//         Albedo.z = albedo.z;
//         Albedo.w = 1.0f;
//     }
    
//     glm::mat4 Model;
//     glm::vec4 Albedo;
//     glm::vec3 RME;
//     int       TextureMask;
// };

// template<>
// inline uint32_t GetDataBinding<ModelData>() { return 2; }

struct MeshData
{    
    glm::mat4 Transform;
};

template<>
inline uint32_t GetDataBinding<MeshData>() { return 2; }

struct MaterialData
{
    MaterialData() = default;
    MaterialData(glm::vec3 albedo, glm::vec2 roughness_metallic, glm::vec3 emission, int mask)
        : RoughnessMetalic(roughness_metallic), Emission(emission), TextureMask(mask)
    {
        Albedo.x = albedo.x;
        Albedo.y = albedo.y;
        Albedo.z = albedo.z;
        Albedo.w = 1.0f;
    }
    
    glm::vec4 Albedo;
    glm::vec2 RoughnessMetalic;
    glm::vec3 Emission;
    float     _padding = 0;
    int       TextureMask;
};

template<>
inline uint32_t GetDataBinding<MaterialData>() { return 3; }

class AssetManager;

class Renderer
{
public:
    Status Init(GraphicsContext* context, GraphicsDevice* device, AssetManager* manager);
    void Terminate();

    void Begin(uint32_t width, uint32_t height, const SceneData& data, const glm::vec3& clear_color);
    void End();

    void BindMeshData(const MeshData& data);
    void BindMaterialData(const MaterialData& data);

    // template<typename VertexT>
    void RenderModel(Mesh& mesh, Material& material, uint32_t textures_slot);

public:
    UniformBuffer SceneBuffer;
    UniformBuffer MeshBuffer;
    UniformBuffer MaterialBuffer;
    Framebuffer   GBufferMultisampled;
    Framebuffer   GBuffer;
    glm::vec3     ClearColor { 0.0f };
    
    Shader* ScreenShader;
    Shader* LightingShader;

private:
    GraphicsContext* m_Context;
    GraphicsDevice* m_Device;
    AssetManager* m_AssetManager;
};

// template<typename VertexT>
inline void Renderer::RenderModel(Mesh& mesh, Material& material, uint32_t textures_slot)
{
    // m_Device->SubmitVertexLayout(GetVertexLayout<VertexT>());
    m_Device->Draw(material, mesh, textures_slot);
}

} // namespace zrn