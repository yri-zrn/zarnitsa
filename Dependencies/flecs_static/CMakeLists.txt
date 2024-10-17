cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0063 NEW)

project(flecs LANGUAGES C)

# option(FLECS_STATIC "Build static flecs lib" ON)
# option(FLECS_SHARED "Build shared flecs lib" ON)
# option(FLECS_PIC "Compile static flecs lib with position independent code (PIC)" ON)
# option(FLECS_TESTS "Build flecs tests" OFF)

# include(cmake/target_default_compile_warnings.cmake)
# include(cmake/target_default_compile_options.cmake)

# file(GLOB_RECURSE INC include/*.h include/*.hpp)
# file(GLOB_RECURSE SRC src/*.c)

# set(FLECS_TARGETS "")

# macro(add_flecs_target TARGET CONFIG)
#     add_library(${TARGET} ${CONFIG} ${INC} ${SRC})
#     add_library(flecs::${TARGET} ALIAS ${TARGET})

#     target_default_compile_options_c(${TARGET})
#     target_default_compile_warnings_c(${TARGET})

#     if(WIN32)
#         target_link_libraries(${TARGET} wsock32 ws2_32)
#     endif()
#     if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#         target_link_libraries(${TARGET} pthread)
#     endif()

#     if(FLECS_PIC)
#         set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
#     endif()

#     target_include_directories(${TARGET} PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#         $<INSTALL_INTERFACE:include>)

#     list(APPEND FLECS_TARGETS ${TARGET})
# endmacro()

# if(FLECS_SHARED)
#     add_flecs_target(flecs SHARED)
# endif()

# if(FLECS_STATIC)
#     add_flecs_target(flecs_static STATIC)
#     target_compile_definitions(flecs_static PUBLIC flecs_STATIC)
# endif()

# if(FLECS_TESTS)
#     enable_testing()
#     add_subdirectory(test)
# endif()

# message(STATUS "Targets: ${FLECS_TARGETS}")

# # define the install steps
# include(GNUInstallDirs)
# install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
#         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#         FILES_MATCHING
#         PATTERN "*.h"
#         PATTERN "*.hpp"
#         PATTERN "*.inl")

# install(TARGETS ${FLECS_TARGETS}
#         EXPORT flecs-export
#         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install(EXPORT flecs-export
#         DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flecs
#         NAMESPACE flecs::
#         FILE flecs-config.cmake)


cmake_minimum_required(VERSION 3.5.0)
project(flecs)

file(GLOB_RECURSE flecs_header include/*.h include/*.hpp)

add_library(flecs INTERFACE
    ${flecs_header}
)

add_library(flecs::flecs_static ALIAS flecs)

target_link_libraries(flecs
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libflecs_static.a
    )

target_include_directories(flecs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(flecs INTERFACE -lWs2_32)

# if(MSVC)
#   target_compile_options(flecs)
# else()
#   target_compile_options(flecs INTERFACE -lWs2_32)
# endif()
# target_compile_definitions( )