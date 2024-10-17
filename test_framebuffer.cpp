#include "Platform.hpp"
#include "PlatformHandle.hpp"
#include "Window.hpp"
#include "Input.hpp"

#include "GraphicsDevice.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "windows.h"

using namespace zrn;
using namespace zrn::Internal;

#pragma region RenderData

struct Vertex
{
    glm::vec3 pos;
    // glm::vec3 col;
    glm::vec2 tex;
};

// { 0.9f, 0.2f, 0.3f }, 
// { 0.1f, 0.9,  0.2f }, 
// { 0.2f, 0.1f, 0.9f }, 
// { 0.9f, 1.0f, 0.2f }, 

static const Vertex vertices[] =
{
    { {  0.5f,  0.5f,  0.0f }, { 0.0f, 0.0f } },  // Lower left
    { {  0.5f, -0.5f,  0.0f }, { 0.0f, 1.0f } },  // Lower right
    { { -0.5f, -0.5f,  0.0f }, { 1.0f, 1.0f } },  // Upper right
    { { -0.5f,  0.5f,  0.0f }, { 1.0f, 0.0f } }   // Upper left
};

static const uint32_t indices[] =
{
    1, 0, 2,
    2, 0, 3
};

static const char* vertex_shader_source =
R"(
    #version 430 core
    layout(location = 0) uniform Matrices
    {
        mat4 MVP;
    } matrices;
    
    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec2 vTex;
    
    // layout(location = 0) out vec3 color;
    layout(location = 0) out vec2 tex_coord;

    void main()
    {
        gl_Position = matrices.MVP * vec4(vPos, 1.0);
        // color = vCol;
        tex_coord = vTex;
    }
)";

static const char* fragment_shader_source =
R"(
    #version 430 core
    // layout(location = 0) in vec3 color;
    layout(location = 0) in vec2 tex_coord;

    layout(location = 0) out vec4 fragment;
    
    uniform sampler2D cat_texture;

    void main()
    {
        // fragment = vec4(color, 1.0);
        // vec3 tex = texture(cat_texture, tex_coord).xyz;
        // fragment = vec4(tex * color, 1.0);
        fragment = texture(cat_texture, tex_coord);
    }
)";

struct Matrices
{
    glm::mat4 MVP;
};

#pragma endregion

void MyErrorCallback(const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Platform::SetErrorCallback(MyErrorCallback);

    Platform::Init();

    Window window {};
    
    Platform::SwapInterval(1);
    Platform::MakeContextCurrent(window);

    GraphicsDevice graphics_device;

    graphics_device.SetErrorCallback(MyErrorCallback);
    graphics_device.Init(GraphicsAPI::OpenGL, nullptr); // (GraphicsProc)PlatformHandle::_GetProcAddress

    Buffer vertex_buffer;
    Attribute layout[] = { ShaderDataType::Float3, ShaderDataType::Float2 };
    BufferLayout vertex_buffer_layout = { layout, 2 };
    graphics_device.CreateVertexBuffer(&vertex_buffer, &vertex_buffer_layout, vertices, sizeof(vertices), false);

    Buffer index_buffer;
    graphics_device.CreateIndexBuffer(&index_buffer, indices, sizeof(indices), false);

    Shader vertex_shader;
    Shader fragment_shader;
    graphics_device.CreateVertexShader(&vertex_shader, vertex_shader_source);
    graphics_device.CreateFragmentShader(&fragment_shader, fragment_shader_source);

    ShaderProgram shader_program;
    graphics_device.CreateShaderProgram(&shader_program);
    graphics_device.AttachShader(&shader_program, &vertex_shader);
    graphics_device.AttachShader(&shader_program, &fragment_shader);
    graphics_device.LinkProgram(&shader_program);

    Buffer uniform_buffer;
    graphics_device.CreateUniformBuffer(&uniform_buffer, nullptr, sizeof(Matrices), false);

    int width, height, channels;
    stbi_uc* image = stbi_load("C:/Zarin/Zarin Engine/neko_sou.png", &width, &height, &channels, 0);
    if(!image) {
        printf("Failed to load image\n");
        exit(1);
    }

    Texture texture;
    TextureSpecification texture_config = {
        (uint32_t)width,
        (uint32_t)height,
        TextureFormat::RGB8
    };
    graphics_device.CreateTexture(&texture, &texture_config, image);
    stbi_image_free(image);

    // GLuint framebuffer;
    // glCreateFramebuffers(1, &framebuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // GLuint framebuffer_color_attachment;
    // glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer_color_attachment);
    // glBindTexture(GL_TEXTURE_2D, framebuffer_color_attachment);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_attachment, 0);

    // GLuint framebuffer_depth;
    // glCreateTextures(framebuffer, 1, &framebuffer_depth);
    // glBindTexture(GL_TEXTURE_2D, framebuffer_depth);
    
    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, 800, 800);
    // // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth, 0);

    // // if (config->ColorAttachmentsCount > 1)
    // // {
    // //     GLenum buffers[16] = {
    // //         GL_COLOR_ATTACHMENT0,
    // //         GL_COLOR_ATTACHMENT1,
    // //         GL_COLOR_ATTACHMENT2,
    // //         GL_COLOR_ATTACHMENT3,
    // //         GL_COLOR_ATTACHMENT4,
    // //         GL_COLOR_ATTACHMENT5,
    // //         GL_COLOR_ATTACHMENT6,
    // //         GL_COLOR_ATTACHMENT7,
    // //         GL_COLOR_ATTACHMENT8,
    // //         GL_COLOR_ATTACHMENT9,
    // //         GL_COLOR_ATTACHMENT10,
    // //         GL_COLOR_ATTACHMENT11,
    // //         GL_COLOR_ATTACHMENT12,
    // //         GL_COLOR_ATTACHMENT13,
    // //         GL_COLOR_ATTACHMENT14,
    // //         GL_COLOR_ATTACHMENT15
    // //     };
    // //     glDrawBuffers(config->ColorAttachmentsCount, buffers);
    // // }
    // // else if (config->ColorAttachmentsCount == 0)
    // // {
    // //     glDrawBuffer(GL_NONE);
    // // }

    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    // {
    //     GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //     printf("Failed to create create framebuffer: framebuffer is incomplete");
    //     return false;
    // }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FramebufferSpecification framebuffer_spec;
    framebuffer_spec.Width  = 800;
    framebuffer_spec.Height = 800;
    framebuffer_spec.ColorAttachmentsCount = 2;
    framebuffer_spec.Attachments[0] = TextureFormat::RGB16F;
    framebuffer_spec.Attachments[1] = TextureFormat::RGB8;
    framebuffer_spec.DepthAttachment = TextureFormat::Depth24Stencil8;
    framebuffer_spec.Samples = 4;
    Framebuffer framebuffer;
    graphics_device.CreateFramebuffer(&framebuffer, &framebuffer_spec);

    graphics_device.SubmitVertexBufferLayout(&vertex_buffer);

    while (!window.CloseRequested())
    {
        Platform::_PollEvents();
        
        auto[width, height] = window.GetFramebufferSize();
        const float ratio = (float) width / (float) height;

        glViewport(0, 0, width, height);
        glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 m {1.0f};
        glm::rotate(m, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 p = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 1.0f, -1.0f);
        glm::mat4 mvp = p * m;
        Matrices matrices { mvp };

        graphics_device.BindShaderProgram(&shader_program);
        graphics_device.BindVertexBuffer(&vertex_buffer, 0, 0);
        
        graphics_device.BindIndexBuffer(&index_buffer, 0);

        graphics_device.BindUniformBuffer(&uniform_buffer, 0);
        graphics_device.EditBufferData(&uniform_buffer, (void*)&matrices);

        // glBindTexture(GL_TEXTURE_2D, texture);
        graphics_device.BindTexture(&texture, 0);

        graphics_device.Draw(3, 0, 0);

        Platform::SwapBuffers(window);
    }

    Platform::Terminate();

    return EXIT_SUCCESS;
}