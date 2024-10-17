#include "SandboxLayer.hpp"

#include "Application.hpp"
#include "DebugLayer.hpp"
#include "Input.hpp"

#include "Scene/Components.hpp"
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

struct BlurData
{
    BlurData() = default;
    BlurData(glm::vec2 direction, float radius)
        : Direction(direction), Radius(radius) { }
    glm::vec2 Direction;
    float Radius;
    float _padding = 0;
};

// struct ColorData
// {
//     glm::vec4 Color;
// };

// struct ScreenData
// {
//     uint32_t ScreenTextureIndex = 0;
// };

namespace zrn
{

bool SandboxLayer::Init()
{
    m_DebugLayer = (DebugLayer*)&Application->GetLayer("DebugLayer");

    World.Init(&Application->Context);

    if (!m_Device.Create(&Application->Context, GraphicsAPI::OpenGL))
        return false;
    
    if (!Application->AssetManager.Init(&Application->Context))
        return false;
    
    return true;
}

void SandboxLayer::Terminate()
{
}

void SandboxLayer::OnBegin()
{
    // ZRN_CORE_ASSERT(false);

    ClearColor = (glm::vec4*)(&m_DebugLayer->ClearColor);
    
    SceneUniformBuffer.Create(&Application->Context, nullptr, sizeof(SceneData));
    ModelUniformBuffer.Create(&Application->Context, nullptr, sizeof(ModelData));
    // BlurUniformBuffer.Create(&Application->Context, nullptr, sizeof(BlurData));
    // ColorUniformBuffer.Create(&Application->Context, nullptr, sizeof(ColorData));
    // ScreenUniformBuffer.Create(&Application->Context, nullptr, sizeof(ScreenData));

    int width, height, channels;
    stbi_uc* image = stbi_load("C:/Zarnitsa/Resources/neko_sou.png", &width, &height, &channels, 0);
    if(!image)
    {
        printf("Failed to load image\n");
    }

    TextureSpecification texture_config = {
        (uint32_t)width,
        (uint32_t)height,
        TextureFormat::RGB8
    };

    TextureAlbedo.Create(&Application->Context, texture_config, image);
    stbi_image_free(image);

    stbi_uc* image2 = stbi_load("C:/Zarnitsa/Resources/neko_sou_roughness.png", &width, &height, &channels, 0);
    if(!image2)
    {
        printf("Failed to load image\n");
    }

    texture_config = {
        (uint32_t)width,
        (uint32_t)height,
        TextureFormat::RGB8
    };

    TextureRoughness.Create(&Application->Context, texture_config, image2);
    stbi_image_free(image2);


    BoxShader = Application->AssetManager.LoadShader(
        &Application->Context,
        "C:/Zarnitsa/Resources/Shaders/Default.glsl"
    );

    GridShader = Application->AssetManager.LoadShader(
        &Application->Context,
        "C:/Zarnitsa/Resources/Shaders/Grid.glsl"
    );

    ScreenShaderMultisampled = Application->AssetManager.LoadShader(
        &Application->Context,
        "C:/Zarnitsa/Resources/Shaders/Screen.glsl"
    );

    ScreenShader = Application->AssetManager.LoadShader(
        &Application->Context,
        "C:/Zarnitsa/Resources/Shaders/ScreenDefault.glsl"
    );

    SolidColorShader = Application->AssetManager.LoadShader(
        &Application->Context,
        "C:/Zarnitsa/Resources/Shaders/SolidColor.glsl"
    );
    
    // OutlineShader = Application->AssetManager.LoadShader(
    //     &Application->Context,
    //     "C:/Zarnitsa/Resources/Shaders/Outline.glsl"
    // );

    Material.Create(
        &Application->Context,
        Application->AssetManager.LoadShader(&Application->Context, "C:/Zarnitsa/Resources/Shaders/Default.glsl")
    );

    Material.Set(TextureAlbedo,    TextureType::Albedo);
    Material.Set(TextureRoughness, TextureType::Roughness);

    FramebufferSpecification fb_spec;
    fb_spec.Width   = 1920;
    fb_spec.Height  = 1080;
    fb_spec.Samples = 4;
    fb_spec.ColorAttachmentsCount = 1;
    fb_spec.Attachments[0] = {
        TextureFormat::RGBA8
    };

    fb_spec.DepthAttachment = TextureFormat::Depth24Stencil8;

    // auto status = m_Device.InternalDevice().CreateFramebuffer(&FramebufferSampled, &fb_spec);
    auto status = FramebufferMultisampled.Create(&Application->Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("Sampled framebuffer created");
    
    fb_spec.Samples = 1;

    // status = m_Device.InternalDevice().CreateFramebuffer(&Framebuffer, &fb_spec);
    status = Framebuffer.Create(&Application->Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("Default framebuffer created");

    fb_spec.Width   = 1920;
    fb_spec.Height  = 1080;
    fb_spec.ColorAttachmentsCount = 2;
    fb_spec.Attachments[0] = TextureFormat::RGBA8;
    fb_spec.Attachments[1] = TextureFormat::RGBA8;

    status = FramebufferOutline.Create(&Application->Context, fb_spec);
    if (status)
        ZRN_CORE_INFO("Outline framebuffer created");

    // fb_spec.ColorAttachmentsCount = 2;
    // fb_spec.Attachments[1] = {
    //     TextureFormat::RGBA8
    // };
    // status = m_Device.InternalDevice().CreateFramebuffer(&DebugFramebuffer, &fb_spec);
    // if (status)
    //     ZRN_CORE_INFO("Debug graphics framebuffer created");

    // m_Device.SubmitVertexLayout(GetVertexLayout<Vertex>());
}

void SandboxLayer::OnUpdate(Timestep timestep)
{
    // m_Device.InternalDevice().BindFramebuffer(&FramebufferSampled, 0);
    auto[width, height] = Application->Window.GetFramebufferSize();

    if (!m_DebugLayer->InputCaptured())
        World.Camera.OnUpdate(timestep);

    FramebufferMultisampled.Bind(1);
    glEnable(GL_DEPTH_TEST);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // glEnable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glStencilMask(0xFF);

    // m_Device.InternalDevice().SetViewport(0, 0, width, height);
    // m_Device.InternalDevice().SetClearColor(ClearColor->r, ClearColor->g, ClearColor->b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    World.OnUpdate(timestep);

    SceneUniformBuffer.Bind(1);
    SceneData scene_data {
        World.Camera.ViewProjection,
        World.Camera.Origin()
    };
    SceneUniformBuffer.EditData((void*)&scene_data);

    // Material.Bind();

    World.Registry.each([this](flecs::entity entity,
                               MeshComponent& mesh,
                               flecs::pair<TransformComponent, GlobalTag> transform)
    {
        ModelUniformBuffer.Bind(2);
        ModelData model_data {
            transform->Tranfsorm(),
            Material.Albedo,
            Material.RME,
            Material.GetTextureAttachmentMask()
        };
        ModelUniformBuffer.EditData((void*)&model_data);

        // TEMP: should move to Mesh.Bind() i guess?
        // static bool layout_submited = false;
        // if (!layout_submited)
        // {
        //     mesh.Mesh.Bind();
        //     layout_submited = true;
        // }
        m_Device.SubmitVertexLayout(GetVertexLayout<Vertex>());

        m_Device.Draw(Material, mesh.Mesh, 3);
        // m_Device.Draw(*SolidColorShader, mesh.Mesh);
    });

    {
        FramebufferMultisampled.Unbind(1);
        FramebufferMultisampled.CopyTo(Framebuffer);
        glDisable(GL_DEPTH_TEST);
    }

#ifdef SELECTED_ENTITY
    {

        if (World.SelectedEntity)
        {
            FramebufferOutline.Bind(0);
            
            // glEnable(GL_STENCIL_TEST);
            // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
            // glStencilFunc(GL_ALWAYS, 1, 0xff);
            // glStencilMask(0xff);

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            // FramebufferOutline.ClearAttachment(0, 0);
            // FramebufferOutline.ClearAttachment(1, 0);

            // glDisable(GL_DEPTH_TEST);

            auto transform = World.SelectedEntity.get<TransformComponent, GlobalTag>();
            auto mesh = World.SelectedEntity.get_mut<MeshComponent>();

            ModelUniformBuffer.Bind(2);
            ModelData model_data {
                transform->Tranfsorm(),
                Material.Albedo,
                Material.RME,
                Material.GetTextureAttachmentMask()
            };
            ModelUniformBuffer.EditData((void*)&model_data);

            mesh->Mesh.Bind();

            // TEMP: should move to Mesh.Bind() i guess?
            m_Device.SubmitVertexLayout(GetVertexLayout<Vertex>());

            // m_Device.Draw(Material, mesh.Mesh);
            
            // ColorData color_data { glm::vec4{1.0, 1.0, 1.0, 1.0}};

            // ColorUniformBuffer.Bind(2);
            // ColorUniformBuffer.EditData(&color_data);
            // glClear(GL_COLOR_BUFFER_BIT);

            // glm::vec4 color{ 0.3f, 0.5f, 0.9f, 1.0f };
            // m_Device.PushConstantData((void*)&color, sizeof(color));

            // m_Device.Draw(*SolidColorShader, mesh->Mesh);

            // ScreenData screen_data { 0 };
            // ScreenUniformBuffer.Bind(1);
            // ScreenUniformBuffer.EditData(&screen_data);
            
            // m_Device.DrawScreen(*OutlineShader);
            
            // glStencilFunc(GL_NOTEQUAL, 1, 0xff);
            // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            // glStencilMask(0x00);

            // color_data.Color = glm::vec4{0.3, 0.5, 0.9, 1.0};
            // ColorUniformBuffer.EditData(&color_data);
            // m_Device.Draw(*SolidColorShader, mesh->Mesh);
            // glPolygonMode()
            
            // for (int i = 0; i < 2; ++i)
            // {
            //     BlurUniformBuffer.Bind(1);
            //     float radius = 8 - i;
            //     BlurData blur_data {
            //         (i%2 == 0 ? glm::vec2{radius, 0.0f} : glm::vec2{0.0f, radius}),
            //         radius
            //     };
            //     BlurUniformBuffer.EditData((void*)&blur_data);
            //     m_Device.DrawScreen(*OutlineShader);
            // }

            // glDisable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);

            FramebufferOutline.Unbind(0);
        }

    }
#endif

    // Render grid to output buffer
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Framebuffer.Bind(1);
        glEnable(GL_DEPTH_TEST);
        m_Device.DrawDummy(6, *GridShader);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        Framebuffer.Unbind(1);
    }

    // Draw screen
    {
        m_Device.InternalDevice().SetClearColor(ClearColor->r, ClearColor->g, ClearColor->b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        Framebuffer.BindAttachments(1);
        uint32_t screen_image_index = 0;
        m_Device.PushConstantData(&screen_image_index, sizeof(uint32_t));
        m_Device.DrawScreen(*ScreenShader);
        glEnable(GL_DEPTH_TEST);
    }
}

void SandboxLayer::OnEvent(Event event)
{
    if (event.Type == EventType::WindowResized || !m_DebugLayer->InputCaptured())
        World.Camera.OnEvent(event);
}

void SandboxLayer::OnEnd()
{
    

}

} // namespace zrn