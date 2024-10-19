#include "Framebuffer.hpp"

#include "GraphicsContext.hpp"

namespace zrn
{

Status Framebuffer::Create(GraphicsContext* context, const FramebufferSpecification& specification)
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
    return device->CreateFramebuffer(&m_Framebuffer, &specification);
}

void Framebuffer::Bind(uint32_t slot)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create framebuffer first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    auto device = m_Context->GetInternalDevice();
    device->BindFramebuffer(&m_Framebuffer, slot);

// TEMP: should not use OpenGL
    glViewport(0, 0, m_Framebuffer.Width, m_Framebuffer.Height);
}

void Framebuffer::Unbind(uint32_t slot)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create framebuffer first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    auto device = m_Context->GetInternalDevice();
    device->BindFramebuffer(nullptr, slot);
}

void Framebuffer::CopyTo(Framebuffer& destination)
{
    ZRN_ON_DEBUG(
        if (!m_Context)
        {
            ZRN_CORE_ERROR("{0}: Must create framebuffer first", ZRN_FUNCTION_NAME);
            return;
        }
    )

    // TEMP
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer.ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.m_Framebuffer.ID);
    
    glBlitFramebuffer(0, 0, m_Framebuffer.Width, m_Framebuffer.Height,
                0, 0, destination.m_Framebuffer.Width, destination.m_Framebuffer.Height,
                GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    for (uint32_t i = 0; i < m_Framebuffer.ColorAttachmentsCount; ++i)
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT0+i);
        glReadBuffer(GL_COLOR_ATTACHMENT0+i);

        glBlitFramebuffer(0, 0, m_Framebuffer.Width, m_Framebuffer.Height,
                          0, 0, destination.m_Framebuffer.Width, destination.m_Framebuffer.Height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    ZRN_NOT_IMPLEMENTED();
}

void Framebuffer::BindAttachments(uint32_t starting_slot)
{
    for (size_t slot = 0; slot < m_Framebuffer.ColorAttachmentsCount; ++slot)
    {
        glBindTextureUnit(starting_slot + slot, m_Framebuffer.ColorAttachments[slot]);
    }
    glBindTextureUnit(starting_slot + m_Framebuffer.ColorAttachmentsCount, m_Framebuffer.DepthAttachment);
}

GLenum GetGLTextureFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGBA8:       return GL_RGBA8;
        case TextureFormat::RGB16F:      return GL_RGB16F;
        case TextureFormat::RedInteger:  return GL_RED_INTEGER;
    default: ZRN_CORE_ASSERT(false, "Invalid texture format");
    }
    return 0;
}

void Framebuffer::ClearAttachment(uint32_t slot, int value)
{
    // glClearTexImage(m_Framebuffer.ColorAttachments[slot], 0, GetGLTextureFormat(m_Framebuffer.ColorAttachmentFormats[slot]), GL_INT, &value);
    // glClearTexImage(m_Framebuffer.ColorAttachments[slot], 0, )
    // for (size_t slot = 0; slot < m_Framebuffer.ColorAttachmentsCount; ++slot)
    // {
    // }
}

DeviceID Framebuffer::GetAttachment(uint32_t slot)
{
    return m_Framebuffer.ColorAttachments[slot];
}

DeviceID Framebuffer::GetDepth()
{
    return m_Framebuffer.DepthAttachment;
}

} // namespace zrn