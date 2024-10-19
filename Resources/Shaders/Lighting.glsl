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

// layout(binding = 1) uniform sampler2D Position;
// layout(binding = 2) uniform sampler2D Albedo;
// layout(binding = 2) uniform sampler2D Albedo;

layout(binding = 1) uniform sampler2D Position;
layout(binding = 2) uniform sampler2D Albedo;
layout(binding = 3) uniform sampler2D RoughnessMetallic;
layout(binding = 4) uniform sampler2D Normal;
layout(binding = 5) uniform sampler2D Emission;

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 fragment;

const vec3 LightColor = vec3(0.3, 0.4, 0.9);
const vec3 LightPos = vec3(3.0, 3.0, 4.0);

void main()
{
    vec3 normal = normalize(texture(Normal, UV).rgb);
    vec3 light_direction = normalize(LightPos - texture(Position, UV).rgb);

    float diffuse = max(dot(normal, light_direction), 0.0);

    vec4 albedo = texture(Albedo, UV);
    fragment = vec4(albedo.rgb * diffuse, albedo.a);
}

#endif