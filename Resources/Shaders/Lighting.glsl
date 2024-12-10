#version 450 core

#define MAX_LIGHT_COUNT 32

const float Pi            = 3.14159265358979323846264338328795;
const float ReciprocalPi  = 0.318309886183790672;
const float Reciprocal2Pi = 0.15915494309189535;

// TEMP
vec3 sRGBtoLinear(vec3 color)
{
    return pow(color, vec3(2.2));
}

vec3 LinearTosRGB(vec3 color)
{
    return pow(color, vec3(1.0/2.2));
}

vec3 ApplyTonemapping(vec3 color, float exposure)
{
    return vec3(1.0) - exp(-color * exposure);
}

vec3 FresnelSchlick(float cos_theta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}

float DGGX(float normal_half, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha*alpha;
    float normal_half2 = normal_half * normal_half;
    float b = (normal_half2 * (alpha2 - 1.0) + 1.0);
    return alpha2 * ReciprocalPi / (b*b); 
}

float G1GGXSchlick(float normal_view, float roughness)
{
    float alpha = roughness * roughness;
    float k = alpha / 2.0;
    return max(normal_view, 0.001) / max(normal_view * (1.0 - k) + k, 0.001);
}

float GSmith(float normal_view, float normal_light, float roughness)
{
    return G1GGXSchlick(normal_light, roughness) * G1GGXSchlick(normal_view, roughness);
}

// Cook-Torrance BRDF
vec3 BRDFMicrofacet(vec3  light_direction,
                    vec3  view_direction,
                    vec3  normal,
                    vec3  albedo,
                    float roughness,
                    float metallic,
                    float specular)
{
    vec3 half_direction = normalize(view_direction + light_direction);

    float normal_view  = clamp(dot(normal, view_direction),  0.0, 1.0);
    float normal_light = clamp(dot(normal, light_direction), 0.0, 1.0);
    float normal_half  = clamp(dot(normal, half_direction),  0.0, 1.0);
    float view_half    = clamp(dot(view_direction, half_direction), 0.0, 1.0);

    vec3 f0 = vec3(0.16 * (specular * specular));
    f0 = mix(f0, albedo, metallic);

    vec3  F = FresnelSchlick(view_half, f0);
    float D = DGGX(normal_half, roughness);
    float G = GSmith(normal_view, normal_light, roughness);

    vec3 spec = (F * D * G) / (4.0 * max(normal_view, 0.001) * max(normal_light, 0.001));

    vec3 rho_D = albedo;

    // Optionally
    rho_D *= vec3(1.0) - F;
    
    rho_D *= (1.0 - metallic);

    vec3 diff = rho_D * ReciprocalPi;
    return diff + spec;
}

// Size: 32
struct PointLight
{
    vec4 Position;
    // float _padding1;
    vec4 Color_RadiantFlux;
};

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec4 CameraPosition;
    // float _padding1;
    vec4 CameraOrigin;
    // float _padding2;
} u_SceneData;

layout(binding = 2) uniform LightingData
{
    PointLight PointLights[MAX_LIGHT_COUNT];
    uint PointLightCount;
    uint _pad1;
    uint _pad2;
    uint _pad3;
} u_LightingData;

#if defined(VERTEX_SHADER)

layout (location = 0) out vec2 UV;
// layout (location = 1) out vec3 WorldSpaceFragmentNormal;

void main() 
{
    // Fullscreen triangle
    UV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(UV * 2.0f + -1.0f, 0.0f, 1.0f);

}

#elif defined(FRAGMENT_SHADER)

// TODO: turn to GBuffer.Position etc.
layout(binding = 1) uniform sampler2D Position;
layout(binding = 2) uniform sampler2D Albedo;
layout(binding = 3) uniform sampler2D RoughnessMetallicSpecular;
layout(binding = 4) uniform sampler2D Normal;
layout(binding = 5) uniform sampler2D Emission;

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 fragment;

void main()
{
    vec3  position  = texture(Position,                  UV).rgb;
    vec3  albedo    = texture(Albedo,                    UV).rgb;
    // float roughness = texture(RoughnessMetallicSpecular, UV).r;
    // float metallic  = texture(RoughnessMetallicSpecular, UV).g;
    // float specular  = texture(RoughnessMetallicSpecular, UV).b;
    float alpha     = texture(Albedo,                    UV).a;
    vec3  normal    = texture(Normal,                    UV).xyz;
    vec3  emission  = texture(Emission,                  UV).rgb;
    
    albedo = sRGBtoLinear(albedo);
    normal = normalize(normal);
    vec3 r_m_s = texture(RoughnessMetallicSpecular, UV).rgb;
    r_m_s = sRGBtoLinear(r_m_s);
    float roughness = r_m_s.r;
    float metallic  = r_m_s.g;
    float specular  = r_m_s.b;

    vec3 radiance = sRGBtoLinear(emission*1.0);

    for (uint i = 0; i < u_LightingData.PointLightCount; ++i)
    {
        vec3  light_position = u_LightingData.PointLights[i].Position.xyz;
        vec3  light_color    = u_LightingData.PointLights[i].Color_RadiantFlux.rgb;
        float light_flux     = u_LightingData.PointLights[i].Color_RadiantFlux.a;

        float light_distance = length(light_position - position);
        vec3  light_direction = normalize(light_position - position);
        vec3  view_direction  = normalize(u_SceneData.CameraPosition.xyz - position);

        float irradiance = max(dot(light_direction, normal), 0.0) * 10.0 * light_flux / (4.0 * Pi * light_distance * light_distance);
        if (irradiance > 0.0)
        {
            vec3 BRDF = BRDFMicrofacet(light_direction, view_direction, normal, albedo, roughness, metallic, specular);
            radiance += BRDF * irradiance * light_color;
        }
    }

    // fragment = vec4(radiance, alpha);
    float exposure = 0.1;
    fragment = vec4(LinearTosRGB(ApplyTonemapping(radiance, exposure)), alpha);
}
#endif