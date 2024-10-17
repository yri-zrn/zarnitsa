#pragma once

#include "GraphicsDevice.hpp"

namespace zrn
{

class Texture
{
public:
    Texture() = default;

    Status Create(GraphicsContext* context, const TextureSpecification& config, const void* data);
    void Bind(uint32_t slot);

private:
    GraphicsContext* m_Context = nullptr;

    Internal::Texture m_Texture;
};

} // namespace zrn