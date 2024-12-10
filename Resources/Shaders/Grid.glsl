#version 450 core

layout(binding = 1) uniform SceneData
{
    mat4 ViewProjection;
    vec4 CameraPosition;
    // float _padding1;
    vec4 CameraOrigin;
    // float _padding2;
} scene_data;


float log10(float x)
{
    float f = log(x) / log(10.0);
    return f;
}

#if defined(VERTEX_SHADER)

const vec3 Pos[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0),
    vec3( 1.0, 0.0, -1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3(-1.0, 0.0,  1.0)
);

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout (location = 0) out vec3 world_position;
layout (location = 1) out float grid_size;

void main() 
{
    // TEMP: move to uniforms
    grid_size = 200.0;

    int Index = Indices[gl_VertexIndex];
    vec3 vPos3 = Pos[Index] * grid_size;
    vPos3.x += scene_data.CameraOrigin.x;
    vPos3.z += scene_data.CameraOrigin.z;

    vec4 vPos = vec4(vPos3, 1.0);
    gl_Position = scene_data.ViewProjection * vPos;

    world_position = vPos3;
}

#elif defined(FRAGMENT_SHADER)

layout(location = 0) in vec3 world_position;
layout (location = 1) in float grid_size;

layout(location = 0) out vec4 fragment;

void main()
{
    float grid_cell_size = 0.025;
    float grid_min_pixels_between_cells = 4.0;

    vec4 grid_color_thin  = vec4(0.4, 0.4, 0.4, 0.3);
    vec4 grid_color_thick = vec4(0.4, 0.4, 0.4, 0.9);

    vec2 dvx = vec2(dFdx(world_position.x), dFdy(world_position.x));
    vec2 dvy = vec2(dFdx(world_position.z), dFdy(world_position.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);

    float l = length(dudv);
    float LOD = max(0.0, log10(l * grid_min_pixels_between_cells / grid_cell_size) + 1.0);

    float grid_cell_size_lod0 = grid_cell_size * pow(10.0, floor(LOD));
    float grid_cell_size_lod1 = grid_cell_size_lod0 * 10.0;
    float grid_cell_size_lod2 = grid_cell_size_lod1 * 10.0;

    dudv *= 3.0;

    vec2 mod_div_dudv = mod(world_position.xz, grid_cell_size_lod0) / dudv;
    vec2 dif = vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0));
    float Lod0a = max(dif.x, dif.y);

    mod_div_dudv = mod(world_position.xz, grid_cell_size_lod1) / dudv;
    dif = vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0));
    float Lod1a = max(dif.x, dif.y);

    mod_div_dudv = mod(world_position.xz, grid_cell_size_lod2) / dudv;
    dif = vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0));
    float Lod2a = max(dif.x, dif.y);

    float LOD_fade = fract(LOD);

    vec4 color;

    if (Lod2a > 0.0)
    {
        color = grid_color_thick;
        color.a *= Lod2a;
    }
    else
    {
        if (Lod1a > 0.0)
        {
            color = mix(grid_color_thick, grid_color_thin, LOD_fade);
            color.a *= Lod1a;
        }
        else
        {
            color = grid_color_thin;
            color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }

    float opacity_falloff = (1.0 - clamp(length(world_position.xz - scene_data.CameraOrigin.xz) / grid_size, 0.0, 1.0));
    color.a *= opacity_falloff;
    fragment = color;
}

#endif