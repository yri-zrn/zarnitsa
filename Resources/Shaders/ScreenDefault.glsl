#version 450 core

layout(push_constant) uniform ConstantBuffer
{
    uint ScreenImageIndex;
} PushConstants;

#if defined(VERTEX_SHADER)

layout (location = 0) out vec2 UV;

void main() 
{
    // Fullscreen triangle
    UV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(UV * 2.0f + -1.0f, 0.0f, 1.0f);
}

#elif defined(FRAGMENT_SHADER)

layout(binding = 1) uniform sampler2D screen_images[8];
layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 fragment;

void main()
{
    fragment = texture(screen_images[PushConstants.ScreenImageIndex], UV);
}

#endif