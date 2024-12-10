#pragma once

#include "Common.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <string>
#include <vector>
#include <map>
#include <optional>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace zrn
{

class Mesh;
class GraphicsContext;

// Contains common meshes and shaders
// Also provides functionality for shader/mesh
// loading from file
class AssetManager
{
public:
    bool Init(GraphicsContext* context);

    void LoadResources(GraphicsContext* context);
    void TryLoadResource(GraphicsContext* context, std::string file_path);
    
    Mesh* LoadMesh(GraphicsContext* context, const std::string& path);
    Mesh* GetMesh(const std::string& name);

    Shader* LoadShader(GraphicsContext* context, const std::string& path);
    Shader* GetShader(const std::string& name);

    Texture* LoadTexture(GraphicsContext* context, const std::string& path);
    Texture* GetTexture(const std::string& name);

    void AddMaterial(Material& material, const std::string& name);
    Material* GetMaterial(const std::string& name);

    std::string GetCacheDirectory() { return m_CacheDirectory; }

    std::string GetResourceDirectory() { return m_ResourceDirectory; }

    const auto& GetMaterialLibrary() const { return m_Materials; }
    auto& GetMaterialLibrary() { return m_Materials; }

    const auto& GetTextureLibrary() const { return m_Textures; }
    auto& GetTextureLibrary() { return m_Textures; }

    const auto& GetMeshLibrary() const { return m_Meshes; }
    auto& GetMeshLibrary() { return m_Meshes; }

private:
    void _CreateCacheDirectory();
    std::string _ReadShaderSource(const std::string& path);
    std::string _GetCachePath(const std::string& shader_original_path, ShaderType type, GraphicsAPI api);
    bool _ShaderCached(const std::string& shader_original_path, ShaderType type);
    bool _ShaderCacheOutdated(const std::string& shader_original_path, ShaderType type);
    void _CacheShader(const std::string& shader_original_path, ShaderType type, GraphicsAPI api, const std::vector<uint32_t> source);

    bool _CompileVulkanBinaries(const std::string& source, ShaderType type, const std::string& name, std::vector<uint32_t>& out_binary);
    bool _CompileOpenGLBinaries(const std::vector<uint32_t>& vulkan_binaries, ShaderType type, const std::string& name, std::vector<uint32_t>& out_binary);

    bool _ReloadShader(const std::string& path, ShaderType type, std::vector<uint32_t>& out_opengl_binary);
    bool _LoadCachedShader(const std::string& path, ShaderType type, std::vector<uint32_t>& out_binary);
    
    void _Reflect(ShaderType type, const std::vector<uint32_t>& binary);

private:
    std::map<std::string, Shader>    m_Shaders;
    std::map<std::string, Mesh>      m_Meshes;
    std::map<std::string, Texture>   m_Textures;
    std::map<std::string, Material>  m_Materials;

    // TEMP
    // TODO: make it configurable
    const std::string m_CacheDirectory = "C:/dev/Zarnitsa/Resources/Cache";
    const std::string m_ResourceDirectory = "C:/dev/Zarnitsa/Resources";
    Assimp::Importer m_Importer;
};

} // namespace zrn