#include "Material.hpp"

#include "Shader.hpp"

#include <string.h>

namespace zrn
{

Status Material::Create(GraphicsContext* context, Shader* shader)
{
    ZRN_ON_DEBUG(
        ErrorType status = ErrorType::NoError;
        if (!context)
        {
            ZRN_CORE_ERROR("{0}: Must provide context", ZRN_FUNCTION_NAME);
            status = ErrorType::ContextNotProvided;
        }
        if (!shader)
        {
            ZRN_CORE_ERROR("{0}: Must provide shader", ZRN_FUNCTION_NAME);
            status = ErrorType::WrongArguments;
        }
        if (status != ErrorType::NoError)
            return status;
    )

    m_Context = context;
    m_Shader = shader;
    memset(m_TextureAttached, false, sizeof(m_TextureAttached));
    memset(m_Textures, 0, sizeof(m_Textures));
    
    return ErrorType::NoError;
}

void Material::Set(Texture& texture, TextureType type)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create material first", ZRN_FUNCTION_NAME);
            return;
        }
        if (type == TextureType::None)
        {
            ZRN_CORE_ERROR("{0}: Wrong texture type", ZRN_FUNCTION_NAME);
            return;
        }
    )

    m_TextureAttached[(size_t)type] = true;
    m_Textures[(size_t)type] = texture;
}

void Material::Bind(uint32_t textures_starting_slot)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create material first", ZRN_FUNCTION_NAME);
            return;
        }
    )
    m_StartingSlot = textures_starting_slot;
    m_Shader->Bind();
    for (size_t type = 0; type < (size_t)TextureType::Last+1; ++type)
    {
        if (m_TextureAttached[type])
        {
            m_Textures[type].Bind(textures_starting_slot + type);
        }
    }
}

int Material::GetTextureAttachmentMask()
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create material first", ZRN_FUNCTION_NAME);
            return 0;
        }
    )
    
    int mask = 0;
    for (size_t type = 0; type < (size_t)TextureType::Last+1; ++type)
    {
        if (m_TextureAttached[type])
        {
            mask |= (1 << type);
        }
    }
    return mask;
}

DeviceID Material::GetTexture(TextureType type)
{
    return m_Textures[(uint32_t)type].GetID();
}

} // namespace zrn