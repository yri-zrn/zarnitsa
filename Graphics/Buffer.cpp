#include "Buffer.hpp"

#include "GraphicsContext.hpp"

// TEMP:
#include "../Common/Common.hpp"

#define _MUST_CREATE_FIRST(resource_name) ZRN_ON_DEBUG(                                  \
    if (!m_Context)                                                                      \
    {                                                                                    \
        ZRN_CORE_ERROR("{0}: Must create {1} first", ZRN_FUNCTION_NAME, #resource_name); \
        return;                                                                          \
    }                                                                                    \
) 

namespace zrn
{

VertexBuffer& VertexBuffer::SetLayout(const VertexLayout& layout)
{
    // _MUST_CREATE_FIRST(vertex buffer)

    m_Layout = layout;
    return *this;
}

void VertexBuffer::SubmitLayout()
{
    _MUST_CREATE_FIRST(vertex buffer)

    auto device = m_Context->GetInternalDevice();
    device->SubmitVertexBufferLayout(&m_Buffer);
}

Status VertexBuffer::Create(GraphicsContext* context, const VertexLayout& layout, const void* data, uint32_t size)
{
    ZRN_ON_DEBUG(
        ErrorType status = ErrorType::NoError;
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            status = ErrorType::ContextNotProvided;
        }
        if (!data || size == 0)
        {
            ZRN_CORE_ERROR("{0}: Must provide data", ZRN_FUNCTION_NAME);
            status = ErrorType::WrongArguments;
        }
        if (layout.AttributeCount == 0)
        {
            ZRN_CORE_ERROR("{0}: Invalid vertex layout", ZRN_FUNCTION_NAME);
            status = ErrorType::WrongArguments;
        }
        if (status != ErrorType::NoError)
            return status;
    )

    m_Context = context;
    auto device = m_Context->GetInternalDevice();

    m_Layout = layout;

    return device->CreateVertexBuffer(&m_Buffer, &m_Layout, data, size, context->ShouldMirror());
}

void VertexBuffer::Bind()
{
    _MUST_CREATE_FIRST(vertex buffer)

    m_Context->AttachedVertexBuffer = this;

    auto device = m_Context->GetInternalDevice();
    device->BindVertexBuffer(&m_Buffer, 0, 0);
}

Status IndexBuffer::Create(GraphicsContext* context, const void* data, uint32_t size)
{
    ZRN_ON_DEBUG(
        ErrorType status = ErrorType::NoError;
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            status = ErrorType::ContextNotProvided;
        }
        if (!data || size == 0)
        {
            ZRN_CORE_ERROR("{0}: Must provide data", ZRN_FUNCTION_NAME);
            status = ErrorType::WrongArguments;
        }
        if (status != ErrorType::NoError)
            return status;
    )

    m_Context = context;

    m_Context->AttachedIndexBuffer = this;

    auto device = m_Context->GetInternalDevice();
    device = context->GetInternalDevice();

    return device->CreateIndexBuffer(&m_Buffer, data, size, false);
}

void IndexBuffer::Bind()
{
    _MUST_CREATE_FIRST(index buffer)
    
    auto device = m_Context->GetInternalDevice();
    device->BindIndexBuffer(&m_Buffer, 0);
}

Status UniformBuffer::Create(GraphicsContext* context, const void* data, uint32_t size)
{
    ZRN_ON_DEBUG(
        ErrorType status{};
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            status = ErrorType::ContextNotProvided;
            return status;
        }
    )

    m_Context = context;

    m_Context->AttachedUniformBuffer = this;

    auto device = m_Context->GetInternalDevice();
    device = context->GetInternalDevice();

    return device->CreateUniformBuffer(&m_Buffer, data, size, context->ShouldMirror());
}

void UniformBuffer::Bind(uint32_t slot)
{
    _MUST_CREATE_FIRST(uniform buffer)

    ZRN_ON_DEBUG(
        if (slot == 0)
        {
            ZRN_CORE_ERROR("Binding 0 is reserved for push constant buffer");
            return;
        }
    )

    auto device = m_Context->GetInternalDevice();
    device->BindUniformBuffer(&m_Buffer, slot);
}

void UniformBuffer::EditData(const void* data)
{
    _MUST_CREATE_FIRST(uniform buffer)

    auto device = m_Context->GetInternalDevice();
    device->EditBufferData(&m_Buffer, data);
}


} // namespace zrn