#version 450 core

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec4 CameraPosition;
    // float _padding1;
    vec4 CameraOrigin;
    // float _padding2;
} scene_data;

layout(binding = 2) uniform ModelData
{
    mat4 Transform;
} model_data;

// ! TODO: FIXME: SOMETHING GOES WRONG HERE, TEXTURE MASK IS INCORRECT 
layout(binding = 3) uniform MaterialData
{
    vec4  Albedo;
    vec4  RoughnessMetallicSpecular;
    // float _padding1;
    vec4  Emission;
    // float _padding2;
    int   TextureMask;
    int   _pad1;
    int   _pad2;
    int   _pad3;
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
    vec4 world_space_vertex_position4 = model_data.Transform * vec4(vPosition, 1.0);

    position    = vec3(world_space_vertex_position4) / world_space_vertex_position4.w;
    normal      = vec3(transpose(inverse(model_data.Transform)) * vec4(vNormal, 0.0));
    tangent     = vTangent;
    tex_coords  = vTexCoords;

    gl_Position = scene_data.ViewProjection * world_space_vertex_position4;
}

#elif defined(FRAGMENT_SHADER)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 tex_coords;

layout(location = 0) out vec4 Position;
layout(location = 1) out vec4 Albedo;
layout(location = 2) out vec4 RoughnessMetallicSpecular;
layout(location = 3) out vec4 Normal;
layout(location = 4) out vec4 Emission;

layout(binding = 4) uniform sampler2D u_Albedo;
layout(binding = 5) uniform sampler2D u_RoughnessMetallicSpecular;
layout(binding = 6) uniform sampler2D u_Normal;
layout(binding = 7) uniform sampler2D u_Emission;

const uint AlbedoIndex                    = 0;
const uint RoughnessMetallicSpecularIndex = 1;
const uint NormalIndex                    = 2;
const uint EmissionIndex                  = 3;

mat3 CotangentFrame(in vec3 normal, in vec3 p, in vec2 UV)
{
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(UV);
    vec2 duv2 = dFdy(UV);

    vec3 dp2_perp = cross(dp2, normal);
    vec3 dp1_perp = cross(normal, dp1);
    vec3 T = dp2_perp * duv1.x + dp1_perp * duv2.x;
    vec3 B = dp2_perp * duv1.y + dp1_perp * duv2.y;

    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, normal);
}

vec3 ApplyNormalMap(in vec3 normal, in vec3 view_direction, in vec2 UV)
{
    vec3 high_res_normal = texture(u_Normal, UV).xyz;
    high_res_normal = normalize(high_res_normal * 2.0 - 1.0);
    mat3 TBN = CotangentFrame(normal, -view_direction, UV);
    return normalize(TBN * high_res_normal);
}

// StartingSlot         = 4
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

    float texture_intensity = material_bit(AlbedoIndex);
    Albedo = mix(material_data.Albedo,
                 texture(u_Albedo, tex_coords),
                 texture_intensity);

    float alpha = Albedo.w;
    
    texture_intensity = material_bit(RoughnessMetallicSpecularIndex);

    RoughnessMetallicSpecular = mix(vec4(material_data.RoughnessMetallicSpecular.rgb, alpha),
                                    texture(u_RoughnessMetallicSpecular, tex_coords),
                                    texture_intensity);
    
    vec3 view_direction = normalize(scene_data.CameraPosition.xyz - position);
    texture_intensity = material_bit(NormalIndex);
    
    Normal = mix(vec4(normal, alpha),
                 vec4(ApplyNormalMap(normal, view_direction, tex_coords), alpha),
                 texture_intensity);
    
    texture_intensity = material_bit(EmissionIndex);
    Emission = mix(vec4(material_data.Emission.rgb, alpha),
                   vec4(texture(u_Emission, tex_coords).rgb, alpha),
                   texture_intensity);
}

#endif