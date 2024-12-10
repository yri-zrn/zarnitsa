#include "SandboxLayer.hpp"

#include "Application.hpp"
#include "DebugLayer.hpp"
#include "Input.hpp"

#include "Scene/Components.hpp"
#include "Mesh.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

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

    World.Init(&Application->Context, &Application->AssetManager);

    if (!Device.Create(&Application->Context, GraphicsAPI::OpenGL))
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
        Application->AssetManager.LoadTexture(&Application->Context, "C:/dev/Zarnitsa/Resources/neko_sou.png");
        Application->AssetManager.LoadTexture(&Application->Context, "C:/dev/Zarnitsa/Resources/neko_sou_roughness.png");
        Application->AssetManager.LoadTexture(&Application->Context, "C:/dev/Zarnitsa/Resources/Wood/WoodRoughness.png");
        Application->AssetManager.LoadTexture(&Application->Context, "C:/dev/Zarnitsa/Resources/Wood/WoodAlbedo.png");
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

        EmptyShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/Empty.glsl"
        );

        SolidColorShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/SolidColor.glsl"
        );

        UVScreenShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/UVScreen.glsl"
        );

        OutlineShader = Application->AssetManager.LoadShader(
            &Application->Context,
            "C:/dev/Zarnitsa/Resources/Shaders/Outline.glsl"
        );
    }

    {
        Material.Create(
            &Application->Context,
            Application->AssetManager.LoadShader(&Application->Context, "C:/dev/Zarnitsa/Resources/Shaders/GBuffer.glsl")
        );

        // Material.Set(*Application->AssetManager.GetTexture("neko_sou"),           TextureType::Albedo);
        // Material.Set(*Application->AssetManager.GetTexture("neko_sou_roughness"), TextureType::RoughnessMetallic);
        Material.Attach(*Application->AssetManager.GetTexture("WoodAlbedo"),           TextureType::Albedo);
        Material.Attach(*Application->AssetManager.GetTexture("WoodRoughness"),        TextureType::RoughnessMetallic);

        Application->AssetManager.AddMaterial(Material, "Cat");
    }

    {
        zrn::Material solid_color;
        solid_color.Create(
            &Application->Context,
            Application->AssetManager.LoadShader(&Application->Context, "C:/dev/Zarnitsa/Resources/Shaders/GBuffer.glsl")
        );
        Application->AssetManager.AddMaterial(solid_color, "Solid color");
    }

    {
        FramebufferSpecification fb_spec;
        fb_spec.Width = 1024;
        fb_spec.Height = 1024;
        fb_spec.ColorAttachmentsCount = 0;
        fb_spec.DepthAttachment = TextureFormat::Depth24Stencil8;
        fb_spec.Samples = 1;
        Depth.Create(&Application->Context, fb_spec);

        fb_spec.Width = 1920;
        fb_spec.Height = 1080;
        fb_spec.ColorAttachmentsCount = 1;
        fb_spec.Attachments[0] = TextureFormat::RGBA8;
        fb_spec.DepthAttachment = TextureFormat::Depth24Stencil8;
        fb_spec.Samples = 1;
        SelectedObjectFramebuffer.Create(&Application->Context, fb_spec);
    }

    {
        auto light = World.CreatePointLight("Point Light 1");
        auto transform = light.get_mut<TransformComponent, GlobalTag>();
        transform->Translation = glm::vec3{10.0f, 10.0f, 10.0f};
        auto point_light_data = light.get_mut<PointLightComponent>();
        point_light_data->Color = glm::vec3(1.0f);
        point_light_data->RadiantFlux = 2000.0f;

        // auto model = World.CreateEntity("")
        // auto cube = World.CreateEntity("Unnamed Entity", *Application->AssetManager.GetMesh("Cube"));
        // cube.get_mut<TransformComponent>()->Translation = {}
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

    SceneData scene_data
    {
        World.Camera.ViewProjection,
        World.Camera.Position,
        World.Camera.Origin()
    };

    std::vector<PointLight> point_lights;

    World.Registry.each([&](flecs::entity entity,
                                PointLightComponent& light,
                                flecs::pair<TransformComponent, GlobalTag> transform)
    {
        point_lights.push_back({transform->Translation, light.Color, light.RadiantFlux});
    });
    
    LightingData lighting_data;
    lighting_data.SetPointLights(point_lights);
    Renderer.SetLightingData(lighting_data);

    // glClearColor(ClearColor->r, ClearColor->g, ClearColor->b, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    Renderer.Begin(width, height, scene_data, *ClearColor);
    {
#define RENDER_SCENE2
#ifdef RENDER_SCENE2
        World.Registry.each([this](flecs::entity entity,
                                MeshComponent& mesh,
                                MaterialComponent& material,
                                flecs::pair<TransformComponent, GlobalTag> transform)
        {
            MeshData mesh_data
            {
                transform->Tranfsorm()
            };

            MaterialData material_data
            {
                material.Material->Albedo,
                material.Material->RoughnessMetallicSpecular,
                material.Material->Emission,
                material.Material->GetTextureAttachmentMask()
            };

            Renderer.SetMeshData(mesh_data);
            Renderer.SetMaterialData(material_data);
            Device.SubmitVertexLayout(GetVertexLayout<Vertex>());
            Renderer.RenderModel(*mesh.Mesh, *material.Material, 4);
        });
#endif
    }
    Renderer.End();

#ifdef SELECTED_ENTITY0
    if (World.SelectedEntity)
    {
        SelectedObjectFramebuffer.Bind(1);
        {
            auto transform = World.SelectedEntity.get<TransformComponent, GlobalTag>();
            auto mesh = World.SelectedEntity.get_mut<MeshComponent>();
            
            MeshData selected_mesh_data
            {
                transform->Tranfsorm()
            };
            
            Renderer.SetSceneData(scene_data);
            Renderer.SetMeshData(selected_mesh_data);
            glEnable(GL_STENCIL_TEST);
            // glDisable(GL_DEPTH_TEST); xx
            // glDepthMask(GL_FALSE); xx
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            // glStencilFunc(GL_EQUAL, 1, 0xFF); xx
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            // glDepthMask(GL_FALSE);

            // glClearStencil(0);

            // glClearDepth(1);
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            Device.SubmitVertexLayout(GetVertexLayout<Vertex>());
            
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            Device.Draw(*EmptyShader, mesh->Mesh);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            // glDepthMask(GL_TRUE);
            // glStencilMask(0x00);
            // glStencilFunc(GL_EQUAL, 1, 0xFF);

            // OutlineFramebuffer.BindAttachments(1);
            // uint32_t stencil_index = 1;
            // Device.PushConstantData(&stencil_index, sizeof(stencil_index));
            // Device.DrawScreen(*OutlineShader);
            // glClearColor(0.0, 0.0, 0.0, 0.0);
            // glClear(GL_COLOR_BUFFER_BIT);
            // Device.DrawScreen(*UVScreenShader);

            glDisable(GL_STENCIL_TEST);

            // glDepthMask(GL_TRUE);
            // glEnable(GL_DEPTH_TEST);
            // glDisable(GL_STENCIL_TEST);
        }
        SelectedObjectFramebuffer.Unbind(1);
    }
#endif

#define RENDER_GRID
#ifdef RENDER_GRID
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Device.DrawDummy(6, *GridShader); // <- causes up to 8 fragment shader recompilations for some reason
    
    
    // SelectedObjectFramebuffer.BindDepth(1);
    // glm::vec4 cutout_color { 1.0 };
    // Device.PushConstantData(&cutout_color, sizeof(cutout_color));
    // Device.DrawScreen(*OutlineShader);

    // Device.DrawScreen(*OutlineShader);
    
    // Device.DrawScreen(*ScreenShader);
    
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