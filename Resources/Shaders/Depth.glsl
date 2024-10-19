#version 450 core

// #include "C:/Zarnitsa/Resources/Shaders/test_include.inl.glsl"

#if defined(VERTEX_SHADER)

layout(push_constant) uniform Constants
{
    mat4 LightSpace;
    mat4 Model;
} DepthData;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aTexCoords;

void main()
{
    gl_Position = DepthData.LightSpace * DepthData.Model * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT_SHADER)

void main()
{
    
}

#endif