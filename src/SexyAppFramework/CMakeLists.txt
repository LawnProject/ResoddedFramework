# CmakeLists.txt
project(SexyAppFramework)

set(SEXY_USE_IMGUI ON)
set(SEXY_USE_CONTROLLER ON)
if(ANDROID OR IOS)
    set(SEXY_CRASH_HANDLER OFF)
else()
    set(SEXY_CRASH_HANDLER ON)
endif()

find_package(SDL3 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
if(SEXY_CRASH_HANDLER)
    find_package(cpptrace CONFIG REQUIRED)
endif()
find_package(CURL CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/thirdparty/src/*.c"
    "${CMAKE_SOURCE_DIR}/thirdparty/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/thirdparty/src/*.h"
)

# popcap...WHYYYY
set(EXCLUDE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangleInc1.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangleInc2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_GetTexel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Loop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Loop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Pixel555.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Pixel565.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Pixel888.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_Pixel8888.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_TexelARGB.cpp"
)

foreach(EXCLUDED_FILE IN LISTS EXCLUDE_FILES)
    list(REMOVE_ITEM SOURCES "${EXCLUDED_FILE}")
endforeach()

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL3::SDL3
    ZLIB::ZLIB
    OpenAL::OpenAL
    glm::glm
    Ogg::ogg
    Freetype::Freetype
    Vorbis::vorbisfile
    Vorbis::vorbis
    $<$<BOOL:${SEXY_CRASH_HANDLER}>:cpptrace::cpptrace>
    CURL::libcurl
    ImageLib
    PakLib
)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/x64")
else()
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/x86")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        bass.lib
        avcodec.lib
        avdevice.lib
        avformat.lib
        avutil.lib
        swresample.lib
        swscale.lib
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
        ${THIRDPARTY_DIR}
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(AVCODEC REQUIRED libavcodec)
    pkg_check_modules(AVDEVICE REQUIRED libavdevice)
    pkg_check_modules(AVFORMAT REQUIRED libavformat)
    pkg_check_modules(AVUTIL REQUIRED libavutil)
    pkg_check_modules(SWRESAMPLE REQUIRED libswresample)
    pkg_check_modules(SWSCALE REQUIRED libswscale)

    target_include_directories(${PROJECT_NAME} PUBLIC
        ${AVCODEC_INCLUDE_DIRS}
        ${AVDEVICE_INCLUDE_DIRS}
        ${AVFORMAT_INCLUDE_DIRS}
        ${AVUTIL_INCLUDE_DIRS}
        ${SWRESAMPLE_INCLUDE_DIRS}
        ${SWSCALE_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${AVCODEC_LIBRARIES}
        ${AVDEVICE_LIBRARIES}
        ${AVFORMAT_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${SWRESAMPLE_LIBRARIES}
        ${SWSCALE_LIBRARIES}
    )

    find_library(BASS_LIBRARY NAMES bass)
    if(BASS_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${BASS_LIBRARY})
    else()
        message(WARNING "BASS library not found by CMake! Falling back to linking dynamically to 'bass'.")
        target_link_libraries(${PROJECT_NAME} PUBLIC bass)
    endif()
elseif(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(jnigraphics-lib jnigraphics)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${log-lib} ${android-lib} ${jnigraphics-lib} GLESv3 EGL)

    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/android/bass/libs/${ANDROID_ABI})
    target_link_libraries(${PROJECT_NAME} PUBLIC bass)
elseif(IOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        "-framework Foundation"
        "-framework UIKit"
        "-framework OpenGLES"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework GameController"
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework Metal"
    )
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB

    # SAF specific
    USE_OGG_LIB
    $<$<NOT:$<BOOL:${ANDROID}>>:SEXY_USE_OPENGL>
    SEXY_USE_SDL3_RENDERER
    $<$<BOOL:${SEXY_USE_IMGUI}>:SEXY_USE_IMGUI>
    $<$<BOOL:${SEXY_USE_CONTROLLER}>:SEXY_USE_CONTROLLER>
    $<$<BOOL:${SEXY_CRASH_HANDLER}>:SEXY_CRASH_HANDLER>

)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SEXY_IS_X64
)
else ()
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SEXY_IS_X86
)
endif()

set(BUILD_INFO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h.in)
set(BUILD_INFO_DST ${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h)

add_custom_target(build_info ALL
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${BUILD_INFO_SRC}
        -DDST=${BUILD_INFO_DST}
        -DSEXY_PROJECT_ROOT=${CMAKE_SOURCE_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/BuildInfo.cmake
    COMMENT "Updating build info..."
)

set_source_files_properties(${BUILD_INFO_DST} PROPERTIES GENERATED TRUE)
target_sources(${PROJECT_NAME} PRIVATE ${BUILD_INFO_DST})
add_dependencies(${PROJECT_NAME} build_info)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/thirdparty/src"
)


foreach(FILE_PATH IN LISTS SOURCES)
    get_filename_component(EXT "${FILE_PATH}" EXT)
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/src/SexyAppFramework" "${FILE_PATH}")

    get_filename_component(FOLDER_PATH "${REL_PATH}" PATH)
    string(REPLACE "/" "\\" FOLDER_PATH "${FOLDER_PATH}")

    if(EXT STREQUAL ".cpp" OR EXT STREQUAL ".c" OR EXT STREQUAL ".cc")
        set(BASE_GROUP "Source Files")
    elseif(EXT STREQUAL ".h" OR EXT STREQUAL ".hpp" OR EXT STREQUAL ".hh")
        set(BASE_GROUP "Header Files")
    elseif(EXT STREQUAL ".rc" OR EXT STREQUAL ".res")
        set(BASE_GROUP "Resource Files")
    else()
        set(BASE_GROUP "Other Files")
    endif()

    if(FOLDER_PATH)
        set(GROUP_NAME "${BASE_GROUP}\\${FOLDER_PATH}")
    else()
        set(GROUP_NAME "${BASE_GROUP}")
    endif()

    source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
endforeach()