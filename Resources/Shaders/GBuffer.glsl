#version 450 core

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec3 CameraPosition;
    float _padding;
} scene_data;

layout(binding = 2) uniform ModelData
{
    mat4 Transform;
} model_data;

layout(binding = 3) uniform MaterialData
{
    vec4  Albedo;
    vec2  RoughnessMetallic;
    vec3  Emission;
    float _padding;
    int   TextureMask;
} material_data;

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
    vec4 pos = model_data.Transform * vec4(vPosition, 1.0);
    gl_Position = scene_data.ViewProjection * pos;

    position    = pos.rgb;
    normal      = mat3(transpose(inverse(model_data.Transform))) * vNormal;
    tangent     = vTangent;
    tex_coords  = vTexCoords;
}

#elif defined(FRAGMENT_SHADER)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 tex_coords;

layout(location = 0) out vec4 Position;
layout(location = 1) out vec4 Albedo;
layout(location = 2) out vec4 RoughnessMetallic;
layout(location = 3) out vec4 Normal;
layout(location = 4) out vec4 Emission;

layout(binding = 4) uniform sampler2D Textures[4];

uint AlbedoIndex            = 0;
uint RoughnessMetallicIndex = 1;
uint NormalIndex            = 2;
uint EmissionIndex          = 3;

// StartingSlot         = 3
// Albedo               =  0 + StartingSlot
// RoughnessMetallic    =  1 + StartingSlot
// Normal               =  2 + StartingSlot
// Emission             =  3 + StartingSlot

float material_bit(uint index)
{
    return (material_data.TextureMask & (1 << index)) >> index;
}

float lineralize_depth(float depth)
{
    float near = 0.01;
    float far = 1000.0;
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

void main()
{
    Position = vec4(position, 0.0);

    float value_intensity = 0.0 - material_bit(AlbedoIndex);
    Albedo = mix(texture(Textures[AlbedoIndex], tex_coords),
                 material_data.Albedo,
                 value_intensity);
    
    value_intensity = 0.0 - material_bit(RoughnessMetallicIndex);
    RoughnessMetallic = mix(texture(Textures[RoughnessMetallicIndex], tex_coords),
                            vec4(material_data.RoughnessMetallic, 0.0, 0.0),
                            value_intensity);
    
    Normal = vec4(normal, 0.0);
    
    value_intensity = 0.0 - material_bit(EmissionIndex);
    Emission = mix(texture(Textures[EmissionIndex], tex_coords),
                   vec4(material_data.Emission, 0.0),
                   value_intensity);
}

#endif