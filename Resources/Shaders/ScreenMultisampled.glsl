#version 450 core

// #include "C:/Zarnitsa/Resources/Shaders/test_include.inl.glsl"

#if defined(VERTEX_SHADER)

layout(binding = 0) uniform SceneData
{
    mat4 ViewProjection;
    vec3 CameraPosition;
    float _padding;
} scene_data;

layout (location = 0) out vec2 UV;

void main() 
{
    // Fullscreen triangle
    UV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(UV * 2.0f + -1.0f, 0.0f, 1.0f);
}

#elif defined(FRAGMENT_SHADER)

vec4 textureMultisample(sampler2DMS _sampler, ivec2 _uv)
{
    vec4 color = vec4(0.0);

    for (int i = 0; i < 4; i++)
        color += texelFetch(_sampler, _uv, i);

    color /= float(4);

    return color;
}

layout(binding = 2) uniform sampler2DMS screen;

layout(location = 0) in vec2 UV;

layout(location = 0) out vec4 fragment;

void main()
{
    // fragment = vec4(0.8, 0.2, 0.3, 1.0);
    // vec2 uv = vec2(0.5, 0.5);
    // ivec2 texSize = textureSize(screen, 0);
    ivec2 texSize = textureSize(screen);
    ivec2 texCoord = ivec2(UV * texSize);

    fragment = textureMultisample(screen, texCoord);
    fragment.a = 1.0;
}

#endif