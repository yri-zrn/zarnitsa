#include "Renderer.hpp"

// TEMP
#include "../Scene/AssetManager.hpp"

namespace zrn
{

Status Renderer::Init(GraphicsContext* context, GraphicsDevice* device, AssetManager* manager)
{
    m_Context = context;
    m_Device = device;
    m_AssetManager = manager;

    SceneBuffer.Create(m_Context, nullptr, sizeof(SceneData));
    MeshBuffer.Create(m_Context, nullptr, sizeof(MeshData));
    MaterialBuffer.Create(m_Context, nullptr, sizeof(MaterialData));
    LightingBuffer.Create(m_Context, nullptr, sizeof(LightingData));

    ScreenShader = m_AssetManager->LoadShader(
        m_Context,
        "C:/dev/Zarnitsa/Resources/Shaders/Screen.glsl"
    );
    LightingShader = m_AssetManager->LoadShader(
        m_Context,
        "C:/dev/Zarnitsa/Resources/Shaders/Lighting.glsl"
    );
    ToneMappingShader = m_AssetManager->LoadShader(
        m_Context,
        "C:/dev/Zarnitsa/Resources/Shaders/ToneMapping.glsl"
    );

    FramebufferSpecification fb_spec;
    fb_spec.Width   = 1920;
    fb_spec.Height  = 1080;
    fb_spec.Samples = 4;
    fb_spec.ColorAttachmentsCount = 5;
    fb_spec.Attachments[0]  = TextureFormat::RGB16F; // Position
    fb_spec.Attachments[1]  = TextureFormat::RGBA8;  // Albedo
    fb_spec.Attachments[2]  = TextureFormat::RGB8;   // RoughnessMetallicSpecular
    fb_spec.Attachments[3]  = TextureFormat::RGB16F; // Normal
    fb_spec.Attachments[4]  = TextureFormat::RGB8;   // Emission
    fb_spec.DepthAttachment = TextureFormat::Depth24Stencil8;

    auto status = GBufferMultisampled.Create(m_Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("Multisampled GBuffer created");
    
    fb_spec.Samples = 1;

    // status = m_Device.InternalDevice().CreateFramebuffer(&Framebuffer, &fb_spec);
    status = GBuffer.Create(m_Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("GBuffer created");

    fb_spec.ColorAttachmentsCount = 1;
    // fb_spec.DepthAttachment = TextureFormat::None;
    fb_spec.Attachments[0]  = TextureFormat::RGBA16F;
    status = Output.Create(m_Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("Output framebuffer created");
    
    return ErrorType::None;
}

void Renderer::Terminate()
{

}

void Renderer::Begin(uint32_t width, uint32_t height, const SceneData& data, const glm::vec3& clear_color)
{
    ClearColor = clear_color;

// #ifdef AAAA
    GBufferMultisampled.Bind(1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    // glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// #endif

    SetSceneData(data);

    // glDisable(GL_DEPTH_TEST);
    // GBuffer.Bind(1);
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::End()
{
    {
        GBufferMultisampled.Unbind(1);
        GBufferMultisampled.CopyTo(GBuffer);
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Output.Bind(1);
    {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        GBuffer.BindAttachments(1);
        LightingBuffer.Bind<LightingData>();
        m_Device->DrawScreen(*LightingShader);
    }
    Output.Unbind(1);

    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, GBuffer.GetID());
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        uint32_t output_index = 0;
        m_Device->PushConstantData(&output_index, sizeof(output_index));
        Output.BindAttachments(1);
        m_Device->DrawScreen(*ScreenShader);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    // glEnable(GL_DEPTH_TEST);
}

void Renderer::SetSceneData(const SceneData& data)
{
    SceneBuffer.Bind<SceneData>();
    SceneBuffer.EditData(&data);
}

void Renderer::SetMeshData(const MeshData& data)
{
    MeshBuffer.Bind<MeshData>();
    MeshBuffer.EditData(&data);
}

void Renderer::SetMaterialData(const MaterialData& data)
{
    MaterialBuffer.Bind<MaterialData>();
    MaterialBuffer.EditData(&data);
}

void Renderer::SetLightingData(const LightingData& data)
{
    LightingBuffer.Bind<LightingData>();
    LightingBuffer.EditData(&data);
}

} // namespace zrn