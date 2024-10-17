#include "Mesh.hpp"

#include "GraphicsContext.hpp"
#include "GraphicsDevice.hpp"

#define _CHECK_STATUS(x) status = x; if (!status) return status;

namespace zrn
{

Status Mesh::Create(GraphicsContext* context, MeshSpecification specification)
{
    ZRN_ON_DEBUG(
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            return ErrorType::ContextNotProvided;
        }
    )
    m_Context = context;
    Status status;
    _CHECK_STATUS(m_VertexBuffer.Create(context, specification.VertexBufferLayout, specification.VertexData, specification.VertexDataSize))
    // _CHECK_STATUS(m_VertexBuffer.SetLayout(specification.VertexBufferLayout))
    _CHECK_STATUS(m_IndexBuffer.Create(context, specification.IndexData, specification.IndexDataSize))
    return ErrorType::NoError;
}

void Mesh::Bind()
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create mesh first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    m_VertexBuffer.Bind();
    m_IndexBuffer.Bind();
}

void Mesh::SubmitLayout()
{
    m_VertexBuffer.SubmitLayout();
}

} // namespace zrn