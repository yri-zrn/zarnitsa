#pragma once

#include "Common.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "Mesh.hpp"
#include "Shader.hpp"

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

    // TEMP
    Mesh LoadModel(const std::string& name, std::string file_path);
    Mesh QuadMesh(GraphicsContext* context);
    Mesh CubeMesh(GraphicsContext* context);
    
    // TEMP: TODO: make it return bool on success
    std::optional<Mesh> LoadMesh(GraphicsContext* context, const std::string& path);
    std::optional<Mesh> GetMesh(const std::string& name);

    Shader* LoadShader(GraphicsContext* context, const std::string& path);
    Shader* GetShader(const std::string& name);

    std::string GetCacheDirectory() { return m_CacheDirectory; }

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
    std::unordered_map<std::string, Shader> m_Shaders;
    std::unordered_map<std::string, Mesh>   m_Meshes;

    // TEMP
    // TODO: make it configurable
    const std::string m_CacheDirectory = "C:/Zarnitsa/Resources/Cache";
    Assimp::Importer m_Importer;
};

} // namespace zrn