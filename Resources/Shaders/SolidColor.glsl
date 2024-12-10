#version 450 core

layout(push_constant) uniform Constants
{
    vec4 Color;
} push_constants;

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec3 CameraPosition;
    float _padding1;
    vec3 CameraOrigin;
    float _padding2;
} scene_data;

layout(binding = 2) uniform ModelData
{
    mat4 Transform;
} model_data;

#if defined(VERTEX_SHADER)

layout(location = 0) in vec3 vPosition;

layout(location = 0) out vec3 position;

void main()
{
    gl_Position = scene_data.ViewProjection * model_data.Transform * vec4(vPosition, 1.0);
    position    = vPosition;
}

#elif defined(FRAGMENT_SHADER)

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 fragment;

void main()
{
    vec3 color = push_constants.Color.rgb;
    color.r = 0.9;
    // fragment = push_constants.Color;
    fragment = vec4(color, 1.0);
}

#endif