#pragma once

#include "Common.hpp"

#include "Internal/GraphicsDevice.hpp"
// #include ""

namespace zrn
{

class GraphicsContext;
class Mesh;
class Shader;
class Material;

class GraphicsDevice
{
private:
    friend GraphicsContext;
    
public:
    GraphicsDevice() = default;

    bool Create(GraphicsContext* context, GraphicsAPI api);
    void Destroy();

    // TEMP?
    void Draw(Shader& shader, Mesh& mesh);
    void Draw(Material& material, Mesh& mesh, uint32_t textures_starting_mask);
    void DrawScreen(Shader& shader);
    void DrawDummy(uint32_t vertex_count, Shader& shader);
    void SubmitVertexLayout(const VertexLayout& layout);

    void PushConstantData(void* data, size_t data_size);
    
    // template<typename T>
    // void SubmitVertexLayout();

    // TEMP
    Internal::GraphicsDevice& InternalDevice() { return m_Device; }

private:
    bool m_Intialized = false;

    // UniformBuffer m_PushConstantBuffer;
    Internal::Buffer m_ConstantBuffer;
    GraphicsContext* m_Context = nullptr;
    Internal::GraphicsDevice m_Device;
};

} // namespace zrn