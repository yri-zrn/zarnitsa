#pragma once

#include "Internal/GraphicsDevice.hpp"
#include "Texture.hpp"

// TEMP
#include "../Platform/Common.hpp"

#include <vector>

enum class TextureType
{
    None               = -1,
    Albedo             =  0,
    RoughnessMetallic  =  1,
    Normal             =  2,
    Emission           =  3,

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
    glm::vec3 Albedo                     = { 0.8f, 0.2f, 0.7f };
    glm::vec3 RoughnessMetallicSpecular  = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Emission                   = { 0.0f, 0.0f, 0.0f };
    uint32_t StartingSlot = 0;
    
    Status Create(GraphicsContext* context, Shader* shader);
    void Attach(Texture& texture, TextureType type);
    void Detach(TextureType type);

    void Bind(uint32_t textures_starting_slot);
    int GetTextureAttachmentMask();
    DeviceID GetTexture(TextureType type);

private:
    GraphicsContext* m_Context = nullptr;
    Shader*          m_Shader = nullptr;
    uint32_t         m_StartingSlot = 0;

    bool      m_TextureAttached[(size_t)TextureType::Last+1];
    Texture   m_Textures[(size_t)TextureType::Last+1];
};

} // namespace zrn