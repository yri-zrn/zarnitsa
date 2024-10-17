cmake_minimum_required(VERSION 3.5.0)
project(assimp)

file(GLOB assimp_header
     "include/*.h"
     "include/*.hpp"
     "include/*.inl"
     "include/*.modulemap"
     "include/Compiler/*.h"
     "include/port/AndroidJNI/*.h"
)

add_library(assimp INTERFACE
    ${assimp_header}
)

target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(assimp::assimp_static ALIAS assimp)

add_subdirectory(contrib)

target_link_libraries(assimp
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.dll.a
        zlib::zlib_static
    )

target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)