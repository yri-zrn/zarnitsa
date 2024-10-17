#version 450 core

// layout(push_constant) uniform constants
// {
// 	vec4 data;
// 	mat4 render_matrix;
// } PushConstants;

#if defined(VERTEX_SHADER)

layout(binding = 1) uniform SceneData
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

layout(binding = 0) uniform sampler2D screen;

layout(location = 0) in vec2 UV;

layout(location = 1) out vec4 fragment;

// float inside(vec2 pos)
// {
//     // ivec2 size = textureSize(screen, 0).xy;
//     return step(0.1, length(texture(screen, pos).rgb));
// }

// bool inside(texture2d tex, vec2 pos)
// {
//   vec3 greenscreen = vec3(0.0);
//   vec3 col = texture(tex, pos).rgb;
//   return length(col - greenscreen) > 0.1;
// }

// #define inside(P) step(.1,length(  texture(screen, (P)/textureSize(screen, 0).xy)  \
//                                  - vec4(.05, .64, .15, 1) ) )

bool inside(vec2 pos)
{
    vec3 greenscreen = vec3(0.0, 0.0, 0.0);
    vec3 col = texture(screen, pos).rgb;
    return length(col - greenscreen) > 0.1;
}

void main()
{
    // vec2 uv = fragCoord;

    vec2 right = vec2(1.0, 0.0);
    vec2 down = vec2(0.0);
    
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 texture_size = textureSize(screen, 0).xy;
    vec4 x = vec4(0.0);
    if (!inside(UV))
    {
        if (inside((fragCoord + right) / texture_size) ||
            inside((fragCoord + down)  / texture_size) ||
            inside((fragCoord - right) / texture_size) ||
            inside((fragCoord - down)  / texture_size))
        {
            // x = vec4(0.3, 0.5, 0.9, 1.0);
            x = vec4(1.0);
        }
    }

    fragment = x;
    // fragment = texture(screen, UV);
}

#endif