#include "AssetManager.hpp"

#include "Components.hpp"

#include "GraphicsContext.hpp"
#include "GraphicsDevice.hpp"

#include <fstream>

#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
// #include <shaderc/glslc/src/file_includer.h>
// #include <shaderc/libshaderc_util/include/libshaderc_util/file_finder.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Sandbox/SandboxLayer.hpp"

namespace zrn
{

static shaderc_shader_kind _GetShadercGLSLShaderKind(ShaderType type)
{
    switch (type)
    {
        case ShaderType::Compute:  return shaderc_glsl_compute_shader;
        case ShaderType::Vertex:   return shaderc_glsl_vertex_shader;
        case ShaderType::Geometry: return shaderc_glsl_geometry_shader;
        case ShaderType::Fragment: return shaderc_glsl_fragment_shader;
        default: break;
    };
    assert(false);
    return (shaderc_shader_kind)0;
}

static std::string GetAPIExtension(GraphicsAPI api)
{
    switch (api)
    {
        case GraphicsAPI::OpenGL: return "gl";
        case GraphicsAPI::Vulkan: return "vk";
        default: break;
    };
    assert(false);
    return "";
}

static std::string GetShaderTypeExtension(ShaderType type)
{
    switch (type)
    {
        case ShaderType::Compute:  return "comp";
        case ShaderType::Vertex:   return "vert";
        case ShaderType::Geometry: return "geom";
        case ShaderType::Fragment: return "frag";
        default: break;
    };
    assert(false);
    return "";
}

static std::string GetShaderTypeMacroDefinition(ShaderType type)
{
    switch (type)
    {
        case ShaderType::Compute:  return "COMPUTE_SHADER";
        case ShaderType::Vertex:   return "VERTEX_SHADER";
        case ShaderType::Geometry: return "GEOMETRY_SHADER";
        case ShaderType::Fragment: return "FRAGMENT_SHADER";
        default: break;
    };
    assert(false);
    return "";
}

bool AssetManager::Init(GraphicsContext* context)
{
    {
        Texture debug_texture;
        TextureSpecification spec;
        spec.Width = 2;
        spec.Height = 2;
        spec.Format = TextureFormat::RGBA8;
        uint32_t data[4] = {
            0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF
        };
        debug_texture.Create(context, spec, data);
        m_Textures["Debug"] = debug_texture;
    }

    {
        Shader debug_shader;
        LoadShader(context, "C:/dev/Zarnitsa/Resources/Shaders/Debug.glsl");
    }
    
    {
        Material debug_material;
        // debug_material.Albedo = {0.9f, 0.04f, 0.9f};
        debug_material.Create(context, GetShader("Debug"));
        debug_material.Attach(*GetTexture("Debug"), TextureType::Albedo);
        m_Materials["Debug"] = debug_material;
    }

    return true;
}

void AssetManager::LoadResources(GraphicsContext* context)
{
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(GetResourceDirectory()))
    {
        if (!dir_entry.is_directory())
        {
            TryLoadResource(context, dir_entry.path().string());
        }
    }
}

void AssetManager::TryLoadResource(GraphicsContext* context, std::string file_path)
{
    auto filename = std::filesystem::path(file_path).stem().string();
    auto extension = std::filesystem::path(file_path).extension().string();

    if (extension == ".spv")
        return;

    if (extension == ".obj" || extension == ".glb" || extension == ".gltf")
    {
        auto mesh = LoadMesh(context, file_path);

    }
    else if (extension == ".png" || extension == ".jpg")
    {
        auto texture = LoadTexture(context, file_path);
    }
}

Mesh* AssetManager::LoadMesh(GraphicsContext* context, const std::string& path)
{
    Mesh result;

    std::vector<Vertex> vertex_data;
    std::vector<uint32_t> index_data;

    static Assimp::Importer importer;
    auto import_flags = aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
    const aiScene* scene = importer.ReadFile(path, import_flags);

    if (scene == nullptr)
    {
        ZRN_CORE_WARN("Failed to load file: {0}", path);
        nullptr;
        
    }

    std::vector<aiMesh*> meshes{ scene->mMeshes, scene->mMeshes + scene->mNumMeshes };

    uint32_t mesh_id = 0;
    for(auto mesh : meshes) {
        std::vector<aiVector3D> vertices{ mesh->mVertices, mesh->mVertices + mesh->mNumVertices };

        uint32_t vertex_id = 0;

        for(const auto& vertex : vertices) {
            const aiVector3D* normals = mesh->mNormals;
            const aiVector3D* tangents = mesh->mTangents;
            const aiVector3D* tex_coords = mesh->mTextureCoords[mesh_id];

            vertex_data.insert(vertex_data.end(),
            {
                { vertex.x, vertex.y, vertex.z },
                { normals[vertex_id].x, normals[vertex_id].y, normals[vertex_id].z },
                { tangents[vertex_id].x, tangents[vertex_id].y, tangents[vertex_id].z },
                { tex_coords[vertex_id].x, 1.0f - tex_coords[vertex_id].y },
            });
            ++vertex_id;
        }

        std::vector<aiFace> faces{ mesh->mFaces, mesh->mFaces + mesh->mNumFaces };
        for(const auto& face : faces) {
            index_data.insert(index_data.end(), face.mIndices, face.mIndices + face.mNumIndices);
        }

        ++mesh_id;
    }
    
    MeshSpecification specifination {
        vertex_data.data(),
        vertex_data.size() * sizeof(Vertex),
        index_data.data(),
        index_data.size() * sizeof(uint32_t),
        GetVertexLayout<Vertex>()
    };

    result.Create(context, specifination);
    auto name = std::filesystem::path(path).stem().string();
    m_Meshes[name] = result;
    return &m_Meshes[name];
}

Mesh* AssetManager::GetMesh(const std::string& name)
{
    if (m_Meshes.find(name) == m_Meshes.end())
    {
        // TEMP
        std::cout << "Failed to get mesh: " << name << '\n';
        return nullptr;
    }
    return &m_Meshes.at(name);
}

Shader* AssetManager::LoadShader(GraphicsContext* context, const std::string& path)
{
    _CreateCacheDirectory();

    Shader result;

    std::vector<uint32_t> opengl_vertex_binaries;
    std::vector<uint32_t> opengl_fragment_binaries;

    if (!_ShaderCached(path, ShaderType::Vertex) || _ShaderCacheOutdated(path, ShaderType::Vertex))
    {
        if (!_ReloadShader(path, ShaderType::Vertex, opengl_vertex_binaries))
            return nullptr;
    }
    else
    {
        if (!_LoadCachedShader(path, ShaderType::Vertex, opengl_vertex_binaries))
            return nullptr;
    }
    
    if (!_ShaderCached(path, ShaderType::Fragment) || _ShaderCacheOutdated(path, ShaderType::Fragment))
    {
        if (!_ReloadShader(path, ShaderType::Fragment, opengl_fragment_binaries))
            return nullptr;
    }
    else
    {
        if (!_LoadCachedShader(path, ShaderType::Fragment, opengl_fragment_binaries))
            return nullptr;
    }

    // TEMP /1/: AssetManager works with OpenGL as main API
    // should be using the current user api instead
    // so it should contain GraphicsContext*
    result.Create(context, opengl_vertex_binaries, opengl_fragment_binaries);
    auto name = std::filesystem::path(path).stem().string();
    m_Shaders[name] = result;
    return &m_Shaders[name];  
}

// TEMP: should return copy
// uses pointers now because that's an easy way to update
// all entities that uses this shader
// better update them malually (so they don't contain ponters)
Shader* AssetManager::GetShader(const std::string& name)
{
    if (m_Shaders.find(name) == m_Shaders.end())
        return nullptr;
    return &m_Shaders.at(name);
}

Texture* AssetManager::LoadTexture(GraphicsContext* context, const std::string& path)
{
    Texture texture;
    int width, height, channels;
    stbi_uc* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if(!image)
    {
        ZRN_CORE_ERROR("Failed to load texture {0}", path);
        return nullptr;
    }

    TextureSpecification texture_config;
    texture_config.Width = width;
    texture_config.Height = height;
    if (channels == 4)
    {
        texture_config.Format = TextureFormat::RGBA8;
    }
    else if (channels == 3)
    {
        texture_config.Format = TextureFormat::RGB8;
    }
    else if (channels == 1)
    {
        texture_config.Format = TextureFormat::Red;
    }
    texture.Create(context, texture_config, image);
    
    stbi_image_free(image);
    auto name = std::filesystem::path(path).stem().string();
    m_Textures[name] = texture;
    return &m_Textures[name];
}

Texture* AssetManager::GetTexture(const std::string& name)
{
    if (m_Textures.find(name) == m_Textures.end())
    {
        ZRN_CORE_ERROR("Failed to get texture {0} ", name);
        return {};
    }
    return &m_Textures.at(name);
}

void AssetManager::AddMaterial(Material& material, const std::string& name)
{
    m_Materials[name] = material;
}

Material* AssetManager::GetMaterial(const std::string& name)
{
    if (m_Materials.find(name) == m_Materials.end())
    {
        ZRN_CORE_ERROR("Failed to get texture {0} ", name);
        nullptr;
    }
    return &m_Materials.at(name);
}

void AssetManager::_CreateCacheDirectory()
{
    if (!std::filesystem::exists(m_CacheDirectory))
        std::filesystem::create_directories(m_CacheDirectory);
}

std::string AssetManager::_ReadShaderSource(const std::string& path)
{
    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary);

    assert(in);

    in.seekg(0, std::ios::end);
    auto size = in.tellg();
    in.seekg(0, std::ios::beg);

    assert(size != -1);

    result.resize((uint32_t)size + 1);
    in.read(result.data(), size);
    in.close();
    result.back() = '\0';

    if (!in)
    {
        ZRN_CORE_ERROR("Failed to read {0} characters", (int)(in.gcount() - size));
    }

    ZRN_CORE_TRACE("Input file size: {0}", (int)size);
    ZRN_CORE_TRACE("Source size: {0}", result.size());
    
    return result;
}

std::string AssetManager::_GetCachePath(const std::string& shader_original_path, ShaderType type, GraphicsAPI api)
{
    std::filesystem::path original_path = shader_original_path;
    std::filesystem::path shader_filepath = m_CacheDirectory;
    shader_filepath /= original_path.stem().string() + "." + GetShaderTypeExtension(type) + "." + GetAPIExtension(api) + ".spv";
    return shader_filepath.string();
}

bool AssetManager::_ShaderCached(const std::string& shader_original_path, ShaderType type)
{
    return std::filesystem::exists(_GetCachePath(shader_original_path, type, GraphicsAPI::Vulkan));
}

bool AssetManager::_ShaderCacheOutdated(const std::string& shader_original_path, ShaderType type)
{
    if (!_ShaderCached(shader_original_path, type))
        return false;
    
    auto shader_time = std::filesystem::last_write_time(shader_original_path);
    auto cached_time = std::filesystem::last_write_time(_GetCachePath(shader_original_path, type, GraphicsAPI::Vulkan));

    return  shader_time > cached_time;
}

void AssetManager::_CacheShader(const std::string& shader_original_path, ShaderType type, GraphicsAPI api, const std::vector<uint32_t> source)
{
    std::ofstream out(_GetCachePath(shader_original_path, type, api) , std::ios::out | std::ios::binary);
    if (out.is_open()) {
        out.write((char*)source.data(), source.size() * sizeof(uint32_t));
        out.flush();
        out.close();
    }
}

bool AssetManager::_CompileVulkanBinaries(const std::string& source, ShaderType type, const std::string& name, std::vector<uint32_t>& out_binary)
{
    // CompileShader(ShaderType, GraphicsAPI::OpenGL, )

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.AddMacroDefinition(GetShaderTypeMacroDefinition(type));
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
    const bool optimize = false;
    if (optimize)
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    
    auto module = compiler.CompileGlslToSpv(source.c_str(), _GetShadercGLSLShaderKind(type), name.c_str(), options);
    ZRN_ON_DEBUG(
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZRN_CORE_ERROR("Failed to compile VK SPIR-V: {0}", module.GetErrorMessage());
            return false;
        }
    )

    out_binary = { module.begin(), module.end() };

    // ZRN_CORE_TRACE("Vulkan shader binary");
    // _Reflect(type, out_binary);

    return true;
}

bool AssetManager::_CompileOpenGLBinaries(const std::vector<uint32_t>& vulkan_binaries, ShaderType type, const std::string& name, std::vector<uint32_t>& out_binary)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    options.SetAutoMapLocations(true);
    options.SetAutoBindUniforms(true);
    // options.Set
    options.SetBindingBase(shaderc_uniform_kind::shaderc_uniform_kind_buffer, 0);
    // options.SetBin
    options.SetPreserveBindings(true);
    // options.AddMacroDefinition(GetShaderTypeMacroDefinition(type));
    const bool optimize = false;
    if (optimize)
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    
    spirv_cross::CompilerGLSL glsl_compiler(vulkan_binaries);
    spirv_cross::CompilerGLSL::Options glsl_options;
    glsl_options.emit_push_constant_as_uniform_buffer = true;
    // glsl_options.
    glsl_compiler.set_common_options(glsl_options);
    // glsl_compiler.set
    // glsl_compiler.set_op

    std::string glsl_source = glsl_compiler.compile();

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(glsl_source.c_str(), _GetShadercGLSLShaderKind(type), name.c_str(), options);
    
    ZRN_ON_DEBUG(
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            ZRN_CORE_ERROR("Failed to compile GL SPIR-V: {0}", module.GetErrorMessage());
            return false;
        }

        spirv_cross::Compiler compiler_2({module.begin(), module.end()});
        spirv_cross::ShaderResources resources = compiler_2.get_shader_resources();
        for (const auto& resource : resources.uniform_buffers)
        {
            uint32_t binding = compiler_2.get_decoration(resource.id, spv::DecorationBinding);
            if (resource.name == "ConstantBuffer" && binding != 0)
            {
                ZRN_CORE_ERROR("Binding 0 is reserved for Constant Buffer");
                return false;
            }
        }
    )

    out_binary = { module.begin(), module.end() };
    // ZRN_CORE_TRACE("OpenGL GLSL source:\n {0}", glsl_source);
    // ZRN_CORE_TRACE("OpenGL shader binary");
    // _Reflect(type, out_binary);
    return true;
}

bool AssetManager::_ReloadShader(const std::string& source_path, ShaderType type, std::vector<uint32_t>& out_opengl_binary)
{
    std::string source   = _ReadShaderSource(source_path);
    ZRN_CORE_TRACE("Vulkan GLSL source:\n {0}", source);

    std::vector<uint32_t> vulkan_binaries;
    if (!_CompileVulkanBinaries(source, type, source_path, vulkan_binaries))
    {
        // ZRN_CORE_TRACE("Vulkan GLSL source:\n {0}", source);
        return false;
    }

    std::vector<uint32_t> opengl_binaries;
    if (!_CompileOpenGLBinaries(vulkan_binaries, type, source_path, opengl_binaries))
    {
        return false;
    }
    
    _CacheShader(source_path, type, GraphicsAPI::Vulkan, vulkan_binaries);
    _CacheShader(source_path, type, GraphicsAPI::OpenGL, opengl_binaries);
    out_opengl_binary = opengl_binaries;
    
    return true;
}

bool AssetManager::_LoadCachedShader(const std::string& source_path, ShaderType type, std::vector<uint32_t>& out_binary)
{
    // std::vector<uint32_t> result;

    // TEMP: same as /1/
    std::ifstream in(_GetCachePath(source_path, type, GraphicsAPI::OpenGL), std::ios::in | std::ios::binary);

    if (!in)
        return false;

    in.seekg(0, std::ios::end);
    auto size = in.tellg();
    if (size == -1)
        return false;

    out_binary.resize(size / sizeof(uint32_t));

    in.seekg(0, std::ios::beg);
    in.read((char*)out_binary.data(), size);
    in.close();
    return true;
}

void AssetManager::_Reflect(ShaderType type, const std::vector<uint32_t>& shader_data)
{
    spirv_cross::Compiler compiler(shader_data);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    // ZRN_CORE_TRACE("OpenGLShader::Reflect - {0}, {1}", Utils::GLShaderStageToString(stage), m_Filepath);
    ZRN_CORE_TRACE("   {0} uniform buffers", resources.uniform_buffers.size());
    ZRN_CORE_TRACE("   {0} resources", resources.sampled_images.size());

    if (!resources.uniform_buffers.empty())
        ZRN_CORE_TRACE("Uniform buffers:");
    for (const auto& resource : resources.uniform_buffers) {
        const auto& buffer_type = compiler.get_type(resource.base_type_id);
        uint32_t buffer_size = compiler.get_declared_struct_size(buffer_type);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        int member_count = buffer_type.member_types.size();

        ZRN_CORE_TRACE("   {0}", resource.name);
        ZRN_CORE_TRACE("      Size    = {0}", buffer_size);
        ZRN_CORE_TRACE("      Binding = {0}", binding);
        ZRN_CORE_TRACE("      Members = {0}", member_count);
    }

    if (!resources.stage_inputs.empty())
        ZRN_CORE_TRACE("Inputs:");
    for (const auto& resource : resources.stage_inputs)
    {
        const auto& type = compiler.get_type(resource.base_type_id);
        uint32_t location = compiler.get_decoration(resource.base_type_id, spv::DecorationLocation);
        // int member_count = type.member_types.size();

        ZRN_CORE_TRACE("   {0}", resource.name);
        ZRN_CORE_TRACE("      Location = {0}", location);
        // ZRN_CORE_TRACE("      Members  = {0}", member_count);
    }

    if (!resources.stage_outputs.empty())
        ZRN_CORE_TRACE("Outputs:");
    for (const auto& resource : resources.stage_outputs)
    {
        const auto& type = compiler.get_type(resource.base_type_id);
        uint32_t location = compiler.get_decoration(resource.base_type_id, spv::DecorationLocation);
        // int member_count = type.member_types.size();

        ZRN_CORE_TRACE("   {0}", resource.name);
        ZRN_CORE_TRACE("      Location = {0}", location);
        // ZRN_CORE_TRACE("      Members  = {0}", member_count);
    }
    if (!resources.sampled_images.empty())
        ZRN_CORE_TRACE("Images:");
    for (const auto& resource : resources.sampled_images)
    {
        const auto& type = compiler.get_type(resource.base_type_id);
        uint32_t location = compiler.get_decoration(resource.base_type_id, spv::DecorationBinding);
        // int member_count = type.member_types.size();

        ZRN_CORE_TRACE("   {0}", resource.name);
        ZRN_CORE_TRACE("      Binding = {0}", location);
        // ZRN_CORE_TRACE("      Members  = {0}", member_count);
    }
}

} // namespace zrn