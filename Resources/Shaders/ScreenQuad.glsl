#version 450 core

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

layout(binding = 0) uniform sampler2D screen;

layout(location = 0) in vec2 UV;

layout(location = 0) out vec4 fragment;

void main()
{
    fragment = texture(screen, UV);
    // fragment = vec4(vec3(gl_FragCoord.z), 1.0);
    // vec4 color = texture(screen, UV);
    // float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    // fragment = vec4(average, average, average, 1.0);
}

#endif