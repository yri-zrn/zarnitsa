#include "Texture.hpp"

#include "GraphicsContext.hpp"

namespace zrn
{

Status Texture::Create(GraphicsContext* context, const TextureSpecification& config, const void* data)
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
    return device->CreateTexture(&m_Texture, &config, data);
}

void Texture::Bind(uint32_t slot)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create texture first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    m_Context->AttachedTexture = this;
    
    auto device = m_Context->GetInternalDevice();
    device->BindTexture(&m_Texture, slot);
}


} // namespace zrn