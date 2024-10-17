#include "GraphicsDevice.hpp"

#include "Internal/GraphicsDevice.hpp"
#include "GraphicsContext.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Material.hpp"

namespace zrn
{

bool GraphicsDevice::Create(GraphicsContext* context, GraphicsAPI api)
{
    if (m_Intialized)
        return true;
    
    m_Context = context;

    if (!m_Device.Init(api))
        return false;
    
    m_Intialized = true;

    context->SetDevice(this);

    const uint32_t VulkanPushConstantBufferSize = 128;
    // m_PushConstantBuffer.Create(m_Context, nullptr, VulkanPushConstantBufferSize);
    bool status = m_Device.CreateUniformBuffer(&m_ConstantBuffer, nullptr, 128, false);
    if (!status)
        ZRN_CORE_ERROR("Failed to create Constant Buffer");
    // m_Device.BindUniformBuffer(&m_ConstantBuffer, 0);

    return true;
}

void GraphicsDevice::Destroy()
{
    m_Context->SetDevice(nullptr);

    m_Intialized = false;
    m_Device.Terminate();
}

void GraphicsDevice::Draw(Shader& shader, Mesh& mesh)
{
    shader.Bind();
    mesh.Bind();
    m_Device.Draw(mesh.FacesCount(), 0, 0);
}

void GraphicsDevice::Draw(Material& material, Mesh& mesh, uint32_t textures_starting_mask)
{
    mesh.Bind();
    material.Bind(textures_starting_mask);
    m_Device.Draw(mesh.FacesCount(), 0, 0);
}

void GraphicsDevice::DrawScreen(Shader& shader)
{
    shader.Bind();
    m_Device.DrawDummy(3);
}

void GraphicsDevice::DrawDummy(uint32_t vertex_count, Shader& shader)
{
    shader.Bind();
    m_Device.DrawDummy(vertex_count);
}

void GraphicsDevice::SubmitVertexLayout(const VertexLayout& layout)
{
    m_Device.SubmitVertexLayout(&layout);
}

void GraphicsDevice::PushConstantData(void* data, size_t data_size)
{
    
    // m_PushConstantBuffer.EditData(data);
    m_Device.BindUniformBuffer(&m_ConstantBuffer, 0);
    // m_Device.EditBufferData(&m_ConstantBuffer, data);
    m_Device.EditBufferDataRange(&m_ConstantBuffer, data, data_size, 0);
}

} // namespace zrn