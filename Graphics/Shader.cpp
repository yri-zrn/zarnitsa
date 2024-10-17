#include "Shader.hpp"

#include "GraphicsDevice.hpp"
#include "Internal/GraphicsDevice.hpp"
#include "GraphicsContext.hpp"

#include <vector>

#define _CHECK_STATUS(x) status = x; if (!status) return status;

namespace zrn
{

Status Shader::Create(GraphicsContext* context, const std::vector<char>& vertex_shader_source, const std::vector<char>& fragment_shader_source)
{
    ZRN_ON_DEBUG(
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            return ErrorType::ContextNotProvided;
        }
    )

    m_Context = context;
    auto device = m_Context->GetInternalDevice();

    Status status = ErrorType::NoError;

    _CHECK_STATUS(device->CreateVertexShader(&m_VertexShader, vertex_shader_source.data()))
    _CHECK_STATUS(device->CreateFragmentShader(&m_FragmentShader, fragment_shader_source.data()))
    _CHECK_STATUS(device->CreateShaderProgram(&m_Program))
    device->AttachShader(&m_Program, &m_VertexShader);
    device->AttachShader(&m_Program, &m_FragmentShader);
    _CHECK_STATUS(device->LinkProgram(&m_Program))

    return ErrorType::NoError;
}

Status Shader::Create(GraphicsContext* context, const std::vector<uint32_t>& vertex_shader_source, const std::vector<uint32_t>& fragment_shader_source)
{
    ZRN_ON_DEBUG(
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            return ErrorType::ContextNotProvided;
        }
    )

    m_Context = context;
    auto device = m_Context->GetInternalDevice();
    
    Status status = ErrorType::NoError;

    _CHECK_STATUS(device->CreateVertexShaderBinary(&m_VertexShader, vertex_shader_source.data(), vertex_shader_source.size() * sizeof(uint32_t)))
    _CHECK_STATUS(device->CreateFragmentShaderBinary(&m_FragmentShader, fragment_shader_source.data(), fragment_shader_source.size()  * sizeof(uint32_t)))
    _CHECK_STATUS(device->CreateShaderProgram(&m_Program))
    device->AttachShader(&m_Program, &m_VertexShader);
    device->AttachShader(&m_Program, &m_FragmentShader);
    _CHECK_STATUS(device->LinkProgram(&m_Program))
    
    return ErrorType::NoError;
}

void Shader::Bind()
{
        ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create shader first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    m_Context->AttachedShader = this;
    
    auto device = m_Context->GetInternalDevice();
    device->BindShaderProgram(&m_Program);
}

} // namespace zrn