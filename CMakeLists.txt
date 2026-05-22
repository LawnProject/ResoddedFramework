# CMakeLists.txt
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(ResoddedFramework LANGUAGES C CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # a stupid fix
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
