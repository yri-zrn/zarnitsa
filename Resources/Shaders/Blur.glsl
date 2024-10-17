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

layout(binding = 1) uniform BlurData
{
    vec2  Direction;
    float Radius;
    float _padding;
} blur_data;

layout(binding = 0) uniform sampler2D screen;

layout(location = 0) in vec2 UV;

layout(location = 0) out vec4 fragment;

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  color += texture(image, uv) * 0.29411764705882354;
  color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;
  return color; 
}

void main()
{
    #ifdef EMMM
    float xs = 1920.0/8.0;
    float ys = 1080.0/8.0;
    
    float r = blur_data.Radius;
    float x, y, xx, yy, rr=r*r, dx, dy, w, w0;
    w0=0.3780/pow(r,1.975);
    
    vec2 p;
    vec4 col=vec4(0.0);
    for (dx = 1.0/xs, x = -r, p.x = 0.5+(UV.x*0.5) + (x*dx);
         x<=r;
         x++, p.x+=dx)
    {
        xx=x*x;
        for (dy = 1.0/ys, y = -r, p.y = 0.5+(UV.y*0.5)+(y*dy);
             y<=r;
             y++, p.y+=dy)
        {
            yy=y*y;
            if (xx+yy<=rr)
            {
                w=w0*exp((-xx-yy)/(2.0*rr));
                col+=texture(screen,p)*w;
            }
        }
    }
    fragment = col;
    #endif
    // fragment = texture(screen,UV);
    fragment = blur5(screen, UV, vec2(1920/8, 1080/8), vec2(1.0, 0.0));
    // fragment = texture(screen, UV);

#ifdef EMMM
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    const float offset = 1.0 / 300.0;  

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    
    vec4 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec4(texture(screen, UV + offsets[i]));
    }
    vec4 color = vec4(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];
    
    fragment = color;
#endif

    // fragment = texture(screen, UV);
    // fragment = vec4(vec3(gl_FragCoord.z), 1.0);
    // vec4 color = texture(screen, UV);
    // float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    // fragment = vec4(average, average, average, 1.0);
}

#endif