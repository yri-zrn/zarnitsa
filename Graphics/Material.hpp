#pragma once

#include "Internal/GraphicsDevice.hpp"
#include "Texture.hpp"

// TEMP
#include "../Platform/Common.hpp"

#include <vector>

enum class TextureType
{
    None       = -1,
    Albedo     =  0,
    Metallic   =  1,
    Roughness  =  2,
    Normal     =  3,
    Emission   =  4,

    First = Albedo,
    Last = Emission,
};

namespace zrn
{

class GraphicsContext;
class Texture;

class Material
{
public:
    Material() = default;

    // TEMP: color value is random
    glm::vec3 Albedo = { 0.8f, 0.2f, 0.7f };
    glm::vec3 RME    = { 0.0f, 0.0f, 0.0f };
    uint32_t StartingSlot = 0;
    
    Status Create(GraphicsContext* context, Shader* shader);
    void Set(Texture& texture, TextureType type);

    void Bind(uint32_t textures_starting_slot);
    int GetTextureAttachmentMask();

private:
    GraphicsContext* m_Context;
    Shader*          m_Shader;

    bool      m_TextureAttached[(size_t)TextureType::Last];
    Texture   m_Textures[(size_t)TextureType::Last];
};

} // namespace zrn