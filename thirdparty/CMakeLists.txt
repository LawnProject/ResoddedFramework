include(FetchContent)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

set(LIBTYPE "STATIC" CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_ENABLE_MODULES OFF CACHE BOOL "Disable OpenAL Soft C++20 modules")

set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable support of bzip2 compressed fonts." FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "Disable support of PNG compressed OpenType embedded bitmaps." FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable support of bzip2 compressed fonts." FORCE)

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Disable zlib examples" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip all install rules" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable building dll zlib" FORCE)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG main
)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG master
)

FetchContent_Declare(
    openal
    GIT_REPOSITORY https://github.com/kcat/openal-soft
    GIT_TAG 1.25.1
)

FetchContent_Declare(
    ogg
    GIT_REPOSITORY https://github.com/teampopwork/ogg
    GIT_TAG master
)

FetchContent_Declare(
    vorbis
    GIT_REPOSITORY https://github.com/teampopwork/vorbis
    GIT_TAG master
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG master
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
    GIT_TAG master
)


FetchContent_MakeAvailable(SDL3 zlib openal ogg vorbis glm freetype)
