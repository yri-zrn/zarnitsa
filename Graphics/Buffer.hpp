#pragma once

#include "GraphicsDevice.hpp"

namespace zrn
{

class VertexBuffer
{
public:
    VertexBuffer() = default;

    Status Create(GraphicsContext* context, const VertexLayout& layout, const void* data, uint32_t size);
    void Bind();

    VertexBuffer& SetLayout(const VertexLayout& layout);
    void SubmitLayout();

private:
    VertexLayout m_Layout;

    GraphicsContext* m_Context = nullptr;
    Internal::Buffer m_Buffer;
};

class IndexBuffer
{
public:
    IndexBuffer() = default;

    Status Create(GraphicsContext* context, const void* data, uint32_t size);
    void Bind();
    uint32_t Count() { return m_Buffer.Size / sizeof(uint32_t); }

private:
    GraphicsContext* m_Context = nullptr;
    Internal::Buffer m_Buffer;
};


struct ConstantBuffer;

template<typename UniformDataType>
uint32_t GetDataBinding();

template<typename ConstantBuffer>
inline uint32_t GetDataBinding() { return 0; }

class UniformBuffer
{
public:
    UniformBuffer() = default;

    Status Create(GraphicsContext* context, const void* data, uint32_t size);
    void Bind(uint32_t slot);

    template<typename UniformDataType>
    void Bind();

    void EditData(const void* data);

private:
    GraphicsContext* m_Context = nullptr;
    Internal::Buffer m_Buffer;
};

template<typename UniformDataType>
inline void UniformBuffer::Bind()
{
    Bind(GetDataBinding<UniformDataType>());
}

} // namespace zrn