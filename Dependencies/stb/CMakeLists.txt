cmake_minimum_required(VERSION 3.5)
project(stb LANGUAGES CXX)

add_library(stb
    STATIC
        stb_image.h
)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)

add_library(stb::stb ALIAS stb)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})