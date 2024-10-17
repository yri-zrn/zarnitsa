cmake_minimum_required(VERSION 3.5.0)
project(glm)

file(GLOB glm_header
     "include/glm/*.hpp"
     "include/glm/detail/*.hpp"
     "include/glm/detail/*.inl"
     "include/glm/ext/*.hpp"
     "include/glm/ext/*.inl"
     "include/glm/gtc/*.hpp"
     "include/glm/gtc/*.inl"
     "include/glm/gtx/*.hpp"
     "include/glm/gtx/*.inl"
     "include/glm/simd/*.h"
)

add_library(glm INTERFACE
    ${glm_header}
)

add_library(glm::glm_static ALIAS glm)

target_link_libraries(glm
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglm.a
    )

target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)