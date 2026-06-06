# CMakeLists.txt
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(ResoddedFramework LANGUAGES C CXX)

# Enable Hot Reload for MSVC compilers if supported.
if(MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # a stupid fix

    # Prevent PDB access conflicts when multiple CL.EXE instances compile in parallel.
    add_compile_options(/FS)
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

include(cmake/UserConfig.cmake)

add_subdirectory(src/ImageLib)
add_subdirectory(src/PakLib)
add_subdirectory(src/SexyAppFramework)

add_subdirectory(src/Lawn)