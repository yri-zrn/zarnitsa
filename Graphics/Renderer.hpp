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
    SceneData(glm::mat4 view_proj, glm::vec3 camera_pos, glm::vec3 camera_origin)
        : ViewProjection(view_proj)//, CameraPosition(camera_pos), CameraOrigin(camera_origin)
    {
        CameraPosition.x = camera_pos.x;
        CameraPosition.y = camera_pos.y;
        CameraPosition.z = camera_pos.z;
        CameraPosition.w = 0.0f;

        CameraOrigin.x = camera_origin.x;
        CameraOrigin.y = camera_origin.y;
        CameraOrigin.z = camera_origin.z;
        CameraOrigin.w = 0.0f;
    }
    
    glm::mat4 ViewProjection;
    glm::vec4 CameraPosition;
    // float _padding1 = 0;
    glm::vec4 CameraOrigin;
    // float _padding2 = 0;
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
    MaterialData(glm::vec3 albedo, glm::vec3 roughness_metallic_specular, glm::vec3 emission, int mask)
        // : RoughnessMetallicSpecular(roughness_metallic_specular), Emission(emission), TextureMask(mask)
        : TextureMask(mask)
    {
        Albedo.x = albedo.x;
        Albedo.y = albedo.y;
        Albedo.z = albedo.z;
        Albedo.w = 1.0f;

        RoughnessMetallicSpecular.x = roughness_metallic_specular.x;
        RoughnessMetallicSpecular.y = roughness_metallic_specular.y;
        RoughnessMetallicSpecular.z = roughness_metallic_specular.z;
        RoughnessMetallicSpecular.w = 0.0f;

        Emission.x = emission.x;
        Emission.y = emission.y;
        Emission.z = emission.z;
        Emission.w = 0.0f;
    }
    
    glm::vec4 Albedo;
    glm::vec4 RoughnessMetallicSpecular;
    // float     _padding1 = 0;
    glm::vec4 Emission;
    // float     _padding2 = 0; // why is this padding necessary?
    int       TextureMask;
    int       _pad1 = 0;
    int       _pad2 = 0;
    int       _pad3 = 0;
};

template<>
inline uint32_t GetDataBinding<MaterialData>() { return 3; }

// sizeof(PointLight) = 32!
struct PointLight
{
    PointLight() = default;
    PointLight(glm::vec3 position, glm::vec3 color, float radiant_flux)
        // : RadiantFlux(radiant_flux)
    {
        Position.x = position.x;
        Position.y = position.y;
        Position.z = position.z;
        Position.w = 0.0f;

        Color_RadiantFlux.x = color.x;
        Color_RadiantFlux.y = color.y;
        Color_RadiantFlux.z = color.z;
        Color_RadiantFlux.w = radiant_flux;
    }

    glm::vec4 Position;
    // float _padding1 = 0;
    glm::vec4 Color_RadiantFlux;
    // float _padding2;
    // float RadiantFlux;
};

struct LightingData
{
    void SetPointLights(const std::vector<PointLight>& point_light)
    {
        if (point_light.size() > MaxLightCount)
        {
            ZRN_CORE_WARN("Failed to copy whole point light data: array is larger than maximum light count {0}", MaxLightCount);
            return;
        }
        PointLightCount = point_light.size() < MaxLightCount ? point_light.size() : MaxLightCount;
        memcpy_s(PointLights, MaxLightCount*sizeof(PointLight), point_light.data(), PointLightCount * sizeof(PointLight));
    }

    inline static const uint32_t MaxLightCount = 32;

    PointLight PointLights[MaxLightCount];
    uint32_t PointLightCount = 0;
    uint32_t _pad1 = 0;
    uint32_t _pad2 = 0;
    uint32_t _pad3 = 0;
};

template<>
inline uint32_t GetDataBinding<LightingData>() { return 2; }

class AssetManager;

class Renderer
{
public:
    Status Init(GraphicsContext* context, GraphicsDevice* device, AssetManager* manager);
    void Terminate();

    void Begin(uint32_t width, uint32_t height, const SceneData& data, const glm::vec3& clear_color);
    void End();

    void SetSceneData(const SceneData& data);
    void SetMeshData(const MeshData& data);
    void SetMaterialData(const MaterialData& data);
    void SetLightingData(const LightingData& data);
    Framebuffer& GetGBuffer() { return GBuffer; }
    Framebuffer& GetOutput() { return Output; }

    UniformBuffer& GetMeshBuffer() { return MeshBuffer; }

    // template<typename VertexT>
    void RenderModel(Mesh& mesh, Material& material, uint32_t textures_slot);

public:
    UniformBuffer SceneBuffer;
    UniformBuffer MeshBuffer;
    UniformBuffer MaterialBuffer;
    UniformBuffer LightingBuffer;
    Framebuffer   GBufferMultisampled;
    Framebuffer   GBuffer;
    Framebuffer   Output;
    glm::vec3     ClearColor { 0.0f };
    
    Shader* ScreenShader;
    Shader* LightingShader;
    Shader* ToneMappingShader;

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