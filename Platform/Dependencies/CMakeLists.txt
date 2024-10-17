cmake_minimum_required(VERSION 3.5.0)
project(GLFW)

add_library(GLFW INTERFACE
    GLFW/include/glfw3.h
    GLFW/include/glfw3native.h
)

target_link_libraries(GLFW INTERFACE libglfw3.a)