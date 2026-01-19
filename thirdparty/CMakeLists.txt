include(FetchContent)

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
    GIT_TAG master
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
    GIT_REPOSITORY https://github.com/freetype/freetype
    GIT_TAG master
)


FetchContent_MakeAvailable(SDL3 zlib openal ogg vorbis glm)

add_library(ZLIB::ZLIB ALIAS zlibstatic)

FetchContent_MakeAvailable(freetype)