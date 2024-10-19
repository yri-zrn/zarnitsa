#include "SandboxLayer.hpp"

#include "Application.hpp"
#include "DebugLayer.hpp"
#include "Input.hpp"

#include "Scene/Components.hpp"
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>



namespace zrn
{

struct DepthData
{
    glm::mat4 LightSpace;
    glm::mat4 Model;
};

bool SandboxLayer::Init()
{
    m_DebugLayer = (DebugLayer*)&Application->GetLayer("DebugLayer");

    World.Init(&Application->Context);

    if (!Device.Create(&Application->Context, GraphicsAPI::OpenGL))
        return false;
    
    if (!Application->AssetManager.Init(&Application->Context))
        return false;
    
    Renderer.Init(&Application->Context, &Device, &Application->AssetManager);

    return true;
}

void SandboxLayer::Terminate()
{

}

void SandboxLayer::OnBegin()
{
    ClearColor = (glm::vec4*)(&m_DebugLayer->ClearColor);
    
    {
        int width, height, channels;
        stbi_uc* image = stbi_load("C:/dev/Zarnitsa/Resources/neko_sou.png", &width, &height, &channels, 0);
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

        stbi_uc* image2 = stbi_load("C:/dev/Zarnitsa/Resources/neko_sou_roughness.png", &width, &height, &channels, 0);
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
    }

    {
        BoxShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/GBuffer.glsl"
        );

        ScreenShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/Screen.glsl"
        );

        GridShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/Grid.glsl"
        );

        DepthShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/Depth.glsl"
        );
    }

    {
        Material.Create(
            &Application->Context,
            Application->AssetManager.LoadShader(&Application->Context, "C:/dev/Zarnitsa/Resources/Shaders/GBuffer.glsl")
        );

        Material.Set(TextureAlbedo,    TextureType::Albedo);
        Material.Set(TextureRoughness, TextureType::RoughnessMetallic);
    }

    {
        FramebufferSpecification fb_spec;
        fb_spec.Width = 1024;
        fb_spec.Height = 1024;
        fb_spec.ColorAttachmentsCount = 0;
        fb_spec.DepthAttachment = TextureFormat::Depth24Stencil8;
        fb_spec.Samples = 0;
        Depth.Create(&Application->Context, fb_spec);
    }
}

void SandboxLayer::OnUpdate(Timestep timestep)
{
    auto[width, height] = Application->Window.GetFramebufferSize();

    if (!m_DebugLayer->InputCaptured())
        World.Camera.OnUpdate(timestep);

    World.OnUpdate(timestep);

#ifdef SHADOW_TEST
    glm::mat4 light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
    glm::mat4 light_view = glm::lookAt(glm::vec3{-2.0f, 4.0f, -1.0f},
                                       glm::vec3{0.0f, 0.0f, 0.0f},
                                       glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_space = light_proj * light_view;

    Depth.Bind(1);
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        // DepthShader->Bind();
        
        World.Registry.each([&](flecs::entity entity,
                                MeshComponent& mesh,
                                flecs::pair<TransformComponent, GlobalTag> transform)
        {
            DepthData depth {
                light_space,
                transform->Tranfsorm()
            };
            m_Device.PushConstantData(&depth, sizeof(depth));

            // m_Device.Draw()
            // DepthShader->Bind();
            // glDrawBuffer(GL_NONE);
            // m_Device.Draw();
            m_Device.Draw(*DepthShader, mesh.Mesh);
            // glReadBuffer(GL_NONE);
        });
    }
    Depth.Unbind(1);

    glClearColor(ClearColor->r, ClearColor->g, ClearColor->b, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // DepthVisShader->Bind();
    Depth.BindAttachments(1);
    uint32_t screen_image_index = 0;
    m_Device.PushConstantData(&screen_image_index, sizeof(uint32_t));
    m_Device.DrawScreen(*ScreenShader);

    // glDisable(GL_DEPTH_TEST);
    // // Depth.BindAttachments(1);
    // uint32_t screen_image_index = 0;
    // m_Device->PushConstantData(&screen_image_index, sizeof(uint32_t));
    // m_Device->DrawScreen(*ScreenShader);
    // glEnable(GL_DEPTH_TEST);
#endif

#define RENDER_SCENE
#ifdef RENDER_SCENE

    SceneData scene_data
    {
        World.Camera.ViewProjection,
        World.Camera.Origin()
    };
    // glClearColor(0, 0, 0, 0);
    Renderer.Begin(width, height, scene_data, *ClearColor);

    World.Registry.each([this](flecs::entity entity,
                               MeshComponent& mesh,
                               flecs::pair<TransformComponent, GlobalTag> transform)
    {
        MeshData mesh_data
        {
            transform->Tranfsorm()
        };

        MaterialData material_data
        {
            Material.Albedo,
            Material.RoughnessMetallic,
            Material.Emission,
            Material.GetTextureAttachmentMask()
        };

        Renderer.BindMeshData(mesh_data);
        Renderer.BindMaterialData(material_data);
        Device.SubmitVertexLayout(GetVertexLayout<Vertex>());
        Renderer.RenderModel(mesh.Mesh, Material, 4);
    });
    Renderer.End();
    
    // Device.InternalDevice().SetClearColor(ClearColor->r, ClearColor->g, ClearColor->b, 1.0f);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthFunc(GL_NEVER);
    Device.DrawDummy(6, *GridShader);
    // glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
#endif

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