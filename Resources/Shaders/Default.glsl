#version 450 core

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec3 CameraPosition;
    float _padding;
} scene_data;

layout(binding = 2) uniform ModelData
{
    mat4   Model;
    vec4   Albedo;
    vec3   RME;
    int    TextureMask;
} model_data;

#if defined(VERTEX_SHADER)

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

layout(location = 0) out vec4 fragment;

layout(binding = 3) uniform sampler2D Textures[5];

uint AlbedoIndex    = 0; 
uint MetallicIndex  = 1;
uint RoughnessIndex = 2;
uint NormalIndex    = 3;
uint EmissionIndex  = 4;

// StartingSlot = 3
// Albedo       =  0 + StartingSlot
// Metallic     =  1 + StartingSlot
// Roughness    =  2 + StartingSlot
// Normal       =  3 + StartingSlot
// Emission     =  4 + StartingSlot

float material_bit(uint index)
{
    return (model_data.TextureMask & (1 << index)) >> index;
}

float lineralize_depth(float depth)
{
    float near = 0.01;
    float far = 1000.0;
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

void main()
{
    float value_intensity = 1.0 - material_bit(AlbedoIndex);
    vec4 color = mix(texture(Textures[AlbedoIndex], tex_coords),
                     model_data.Albedo,
                     value_intensity);
    
    // value_intensity = 1.0 - material_bit(RoughnessIndex);
    // color = mix(texture(Textures[RoughnessIndex], tex_coords),
    //             vec4(vec3(model_data.RME[0]), 1.0),
    //             value_intensity);

    fragment = color;
    // fragment = vec4(vec3(lineralize_depth(gl_FragCoord.z)), 1.0);
    // fragment = vec4(tex_coords, 1.0, 1.0);
}

#endif