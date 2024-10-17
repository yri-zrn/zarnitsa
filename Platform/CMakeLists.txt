cmake_minimum_required(VERSION 3.5.0)
project(zrn_Platform)

add_library(zrn_Platform
    Platform.hpp
    Platform.cpp
    Window.hpp
    Window.cpp
    Input.hpp
    Input.cpp
    Events.hpp
    Events.cpp
    FileWatcher.hpp
    # FileWatcher.cpp

    Internal/Common.hpp
    Internal/FileWatcher.hpp
    # Internal/FileWatcher.cpp
    )
add_library(zrn::Platform ALIAS zrn_Platform)

target_include_directories(zrn_Platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Dependencies)

target_link_libraries(zrn_Platform
    PRIVATE
        GLFW
)

if (ZRN_BUILD_WIN32)
    target_sources(zrn_Platform
    PRIVATE
        Internal/Win32/FileWatcherWin32.hpp
        Internal/Win32/FileWatcherWin32.cpp
        )
endif()
