#version 450 core

#if defined(VERTEX_SHADER)

layout(binding = 0) uniform SceneData
{
    mat4 ViewProjection;
    vec3 CameraPosition;
} scene_data;

layout(binding = 1) uniform ModelData
{
    mat4 Model;
    vec3 Albedo;
    vec3 RME;
    int  TextureMask;
} model_data;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vTexCoords;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 tangent;
layout(location = 3) out vec2 tex_coords;

void main() 
{
    gl_Position = scene_data.ViewProjection * model_data.Model * vec4(vPosition, 1.0);
    
    position    = vPosition;
    normal      = vNormal;
    tangent     = vTangent;
    tex_coords  = vTexCoords;
}

#elif defined(FRAGMENT_SHADER)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 tex_coords;

layout(binding = 1) uniform ModelData
{
    mat4 Model;
    vec3 Albedo;
    vec3 RME;
    int  TextureMask;
} model_data;

layout(binding = 0) uniform sampler2D Albedo;
layout(binding = 1) uniform sampler2D Metallic;
layout(binding = 2) uniform sampler2D Roughness;
layout(binding = 3) uniform sampler2D Normal;
layout(binding = 4) uniform sampler2D Emission;

layout(location = 0) out vec4 fragment;

void main()
{
    fragment = vec4(model_data.Albedo, 1.0);
}

#endif