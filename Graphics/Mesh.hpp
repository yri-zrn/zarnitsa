#pragma once

#include "Internal/GraphicsDevice.hpp"
#include "Buffer.hpp"

#include <vector>

namespace zrn
{

template<typename VertexType>
VertexLayout GetVertexLayout();

class GraphicsContext;

class Mesh
{
public:
    Mesh() = default;

    Status Create(GraphicsContext* context, MeshSpecification specification);

    void Bind();

    void SubmitLayout();

    uint32_t FacesCount() { return IndexCount() / 3; }
    uint32_t IndexCount() { return m_IndexBuffer.Count(); }

private:
    GraphicsContext* m_Context = nullptr;
    VertexBuffer m_VertexBuffer;
    IndexBuffer  m_IndexBuffer;
};

} // namespace zrn