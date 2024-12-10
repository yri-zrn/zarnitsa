#version 450 core

layout(push_constant) uniform ConstantBuffer
{
    vec4 CutOutColor;
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

layout(binding = 1) uniform sampler2D image;
layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 fragment;

float Circle(float radius, vec2 position)
{
    return step(radius, distance(position, vec2(0.5)));
}

void main()
{
    // vec2 offset = vec2(1.0/1920.0, 1.0/1080.0);
    // vec4 col = texture2D(image, UV);
	// if (col.r == 1.0)
    // {
	// 	float a = texture2D(image, vec2(UV.x + offset.x, UV.y           )).r +
	// 		      texture2D(image, vec2(UV.x,            UV.y - offset.y)).r +
	// 		      texture2D(image, vec2(UV.x - offset.x, UV.y           )).r +
	// 		      texture2D(image, vec2(UV.x,            UV.y + offset.y)).r;
	// 	if (col.a < 1.0 && a > 0.0)
	// 		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.8);
	// 	else
	// 		gl_FragColor = col;
	// }

#ifdef OLD
    float outline_thickness = 0.2;
    vec3 outline_color = vec3(1.0, 0.9, 0.1);
    float outline_threshold = 0.5;

    vec4 color = texture(image, UV);
    
    fragment = vec4(0.0);
    if (color.r >= 1.0)
    {
        ivec2 size = textureSize(image, 0);

        // float uv_x = UV.x * size.x;
        // float uv_y = UV.y * size.y;
        float uv_x = gl_FragCoord.x/size.x;
        float uv_y = gl_FragCoord.y/size.y;

        // vec2 tex_coord = gl_FragCoord.xy/size;

        float sum = 0.0;
        for (int n = 0; n < 9; ++n)
        {
            uv_y = (UV.y * size.y) + (outline_thickness * float(n - 4.5));
            float h_sum = 0.0;
            h_sum += texelFetch(image, ivec2(uv_x - (4.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x - (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x - (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x - outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x, uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x + outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x + (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x + (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(image, ivec2(uv_x + (4.0 * outline_thickness), uv_y), 0).a;
            sum += h_sum / 9.0;
        }

        if (sum / 9.0 >= 0.0001) {
            fragment = vec4(outline_color, 1);
        }
    }
#endif

// #ifdef OLD_2
    // ivec2 size = textureSize(image, 0);
    // ivec2 size = ivec2(1920, 1080);
    // vec2 pixel_coord = UV * size;
    // vec2 pixel_coord = gl_FragCoord.xy/size;
    // float border_width = 0.005;
    float circle_width = 0.02;

    vec4 color = texture(image, UV);

    fragment = vec4(0.0);

    vec2 offset = vec2(1.0/1920.0, 1.0/1080.0);

    if (color.r < 1.0)
    {
        fragment = vec4(0.2, 0.9, 0.5, 1.0);
    }
    else
    {
        if (Circle(circle_width, vec2(0.1 + UV.x - offset.x, UV.y           )) == 0.0
         || Circle(circle_width, vec2(0.1 + UV.x + offset.x, UV.y           )) == 0.0
         || Circle(circle_width, vec2(0.1 + UV.x           , UV.y + offset.y)) == 0.0
         || Circle(circle_width, vec2(0.1 + UV.x           , UV.y - offset.y)) == 0.0)
        {
            fragment = vec4(0.3, 0.5, 0.9, 1.0);
        }
    }
    // fragment = vec4(UV, 0.0, 1.0);
// #endif
}

#endif