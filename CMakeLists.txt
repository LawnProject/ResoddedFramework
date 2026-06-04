cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# Force MSVC to use Static CRT (MT/MTd) to match x64-windows-static vcpkg dependencies
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(ResoddedFramework LANGUAGES C CXX)

# Enable Hot Reload for MSVC compilers if supported.
if(MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

option(GITHUB_ACTIONS "Building in GitHub Actions" OFF)

if(MSVC AND GITHUB_ACTIONS)
    message(STATUS "GitHub Actions, adding /SAFESEH:NO, screw you BASS")
    add_link_options(/SAFESEH:NO)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# woohooo now we can use folder filters
set(CMAKE_USE_FOLDERS ON)

add_subdirectory(src/ImageLib)
add_subdirectory(src/PakLib)
add_subdirectory(src/SexyAppFramework)

add_subdirectory(src/Lawn)
