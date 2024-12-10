#pragma once

#include "GraphicsDevice.hpp"

namespace zrn
{

class Framebuffer
{
public:
    Framebuffer() = default;

    Status Create(GraphicsContext* context, const FramebufferSpecification& specification);
    void Bind(uint32_t slot);
    void Unbind(uint32_t slot);
    void CopyTo(Framebuffer& destination);
    void Resize(uint32_t width, uint32_t height);
    
    void BindAttachments(uint32_t starting_slot);
    void BindDepth(uint32_t slot);
    void ClearAttachment(uint32_t slot, int value);
    DeviceID GetAttachment(uint32_t slot);
    DeviceID GetDepth();
    DeviceID GetStencil();
    
    DeviceID GetID() const { return m_Framebuffer.ID; }

// private:
    GraphicsContext* m_Context = nullptr;
    Internal::Framebuffer m_Framebuffer;
};

} // namespace zrn