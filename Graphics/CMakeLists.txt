cmake_minimum_required(VERSION 3.5.0)
project(zrn_Graphics)

add_library(zrn_Graphics

    GraphicsDevice.hpp
    GraphicsDevice.cpp
    GraphicsContext.hpp
    GraphicsContext.cpp
    Shader.hpp
    Shader.cpp
    Buffer.hpp
    Buffer.cpp
    Framebuffer.hpp
    Framebuffer.cpp
    Texture.hpp
    Texture.cpp
    Mesh.hpp
    Mesh.cpp
    Material.hpp
    Material.cpp
    Renderer.hpp
    Renderer.cpp

    Internal/GraphicsDevice.hpp
    Internal/GraphicsDevice.cpp
    Internal/Internal.hpp

    # OpenGL
    Internal/OpenGL/GraphicsDeviceOpenGL.hpp
    Internal/OpenGL/GraphicsDeviceOpenGL.cpp

    Internal/OpenGL/glad/glad.c
    Internal/OpenGL/glad/glad.h
    Internal/OpenGL/glad/khrplatform.h
    )

add_library(zrn::Graphics ALIAS zrn_Graphics)

target_include_directories(zrn_Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(zrn_Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Internal)



# if (ZRN_BUILD_WIN32)
# endif()