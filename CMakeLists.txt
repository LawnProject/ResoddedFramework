# CMakeLists.txt
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(GardenersDecomp LANGUAGES C CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
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

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Disable zlib examples" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip all install rules" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable building dll zlib" FORCE)

add_subdirectory(thirdparty)
add_subdirectory(src/ImageLib)
add_subdirectory(src/PakLib)
add_subdirectory(src/SexyAppFramework)

add_subdirectory(src/Lawn)
