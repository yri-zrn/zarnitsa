cmake_minimum_required(VERSION 3.17.0)
project(Zarin)
include(CMakeDependentOption)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Ccache found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
else()
    message(STATUS "Ccache not found")
endif()

cmake_dependent_option(ZRN_BUILD_WIN32 "Win32 Support" ON "WIN32" OFF)

if (ZRN_BUILD_WIN32)
    add_compile_definitions(ZRN_WIN32)
    message(STATUS "Building Win32 platform")
endif()

add_executable(Zarin
    main.cpp

    stb_image.h
    Application/Application.hpp
    Application/Application.cpp
    Application/Layer.hpp
    
    Application/ApplicationLayer.hpp
    Application/ImGuiLayer/ImGuiLayer.hpp
    Application/ImGuiLayer/ImGuiLayer.cpp

    Sandbox/SandboxLayer.hpp
    Sandbox/SandboxLayer.cpp
    Sandbox/DebugLayer.hpp
    Sandbox/DebugLayer.cpp

    Application/ImGuiLayer/imgui_impl_glfw.h
    Application/ImGuiLayer/imgui_impl_glfw.cpp
    Application/ImGuiLayer/imgui_impl_opengl3.h
    Application/ImGuiLayer/imgui_impl_opengl3.cpp
    Application/ImGuiLayer/imgui_impl_opengl3_loader.h

    Common/Timestep.hpp
    Common/Math.hpp
    Common/Logger.hpp
    Common/Logger.cpp
    Common/Macro.hpp
    Common/Status.hpp
    Common/Assert.hpp

    Scene/ViewportCamera.hpp
    Scene/ViewportCamera.cpp
    Scene/World.hpp
    Scene/World.cpp
    Scene/AssetManager.hpp
    Scene/AssetManager.cpp

    Editor/PropertyEditor.cpp
    Editor/PropertyEditor.hpp
    )

add_compile_definitions(
    $<$<CONFIG:Debug>:ZRN_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:ZRN_DEBUG>
    $<$<CONFIG:Release>:ZRN_RELEASE>
    $<$<CONFIG:MinSizeRel>:ZRN_RELEASE>
)

if(MSVC)
  target_compile_options(Zarin PRIVATE /W4 /WX)
else()
  target_compile_options(Zarin PRIVATE -Wall -Wextra -Wpedantic)
endif()


add_subdirectory(${CMAKE_SOURCE_DIR}/Platform)
add_subdirectory(${CMAKE_SOURCE_DIR}/Graphics)
add_subdirectory(Dependencies)

find_package(spirv_cross_c_shared)

target_link_libraries(Zarin
    PUBLIC
        zrn::Platform
        zrn::Graphics
)

target_link_libraries(Zarin
    PUBLIC
        glm::glm_static
        flecs::flecs_static
        assimp::assimp_static
        imgui::imgui
        spdlog::spdlog_static

        # TEMP
        spirv-cross-c-shared
        $ENV{VULKAN_SDK}/Lib/shaderc_shared.lib
        # $ENV{VULKAN_SDK}/Lib/spirv-cross-c-shared.lib
        # $ENV{VULKAN_SDK}/Lib/spirv-cross-c.lib
        # $ENV{VULKAN_SDK}/Lib/spirv-cross-c.lib
        # $ENV{VULKAN_SDK}/Lib/spirv-cross-cpp.lib
)

target_include_directories(Zarin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Zarin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Application)