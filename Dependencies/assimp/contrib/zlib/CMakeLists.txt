cmake_minimum_required(VERSION 3.5.0)
project(zlib)

file(GLOB zlib_header
     "include/*.h"
)

add_library(zlib INTERFACE
    ${zlib_header}
)

add_library(zlib::zlib_static ALIAS zlib)

target_link_libraries(zlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libzlibstatic.a)

target_include_directories(zlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)