cmake_minimum_required(VERSION 3.5.0)
project(spdlog)

file(GLOB spdlog_header
     "include/spdlog/*.h"
     "include/spdlog/cfg/*.h"
     "include/spdlog/details/*.h"
     "include/spdlog/fmt/*.h"
     "include/spdlog/fmt/bundled/*.h"
     "include/spdlog/sinks/*.h"
)

add_library(spdlog INTERFACE
    ${spdlog_header}
)

add_library(spdlog::spdlog_static ALIAS spdlog)

target_link_libraries(spdlog
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libspdlog.a
    )

target_include_directories(spdlog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)