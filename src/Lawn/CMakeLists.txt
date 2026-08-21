project(LawnProject)

option(LAWN_SHOW_TERMINAL_DEBUG "Show terminal output in debug mode when double-clicking to start the game" ON)
option(LAWN_SHOW_TERMINAL_RELEASE "Show terminal output in release mode when double-clicking to start the game" OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"

    "${CMAKE_SOURCE_DIR}/src/TodLib/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/TodLib/*.h"
)

set(EXCLUDE_FILES
    "${CMAKE_SOURCE_DIR}/src/TodLib/TodDrawTriangle.cpp"
    "${CMAKE_SOURCE_DIR}/src/TodLib/TodDrawTriangleInc.cpp"
)

foreach(EXCLUDED_FILE IN LISTS EXCLUDE_FILES)
    list(REMOVE_ITEM SOURCES "${EXCLUDED_FILE}")
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES} LawnProject.rc)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/src" # for <SexyAppFramework/Header.h>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
if (WIN32)
    if (MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<AND:$<CONFIG:Debug>,$<BOOL:${LAWN_SHOW_TERMINAL_DEBUG}>>:/SUBSYSTEM:CONSOLE>
            $<$<AND:$<CONFIG:Release>,$<BOOL:${LAWN_SHOW_TERMINAL_RELEASE}>>:/SUBSYSTEM:CONSOLE>
            $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${LAWN_SHOW_TERMINAL_DEBUG}>>>:/SUBSYSTEM:WINDOWS>
            $<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${LAWN_SHOW_TERMINAL_RELEASE}>>>:/SUBSYSTEM:WINDOWS>
        )
    elseif(MINGW)
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<AND:$<CONFIG:Debug>,$<BOOL:${LAWN_SHOW_TERMINAL_DEBUG}>>:-mconsole>
            $<$<AND:$<CONFIG:Release>,$<BOOL:${LAWN_SHOW_TERMINAL_RELEASE}>>:-mconsole>
            $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${LAWN_SHOW_TERMINAL_DEBUG}>>>:-mwindows>
            $<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${LAWN_SHOW_TERMINAL_RELEASE}>>>:-mwindows>
        )
    endif()
endif()

if(SEXY_USE_IMGUI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:Debug>:LAWN_DEBUG_TOOLS>
    )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    SexyAppFramework
)

foreach(FILE_PATH IN LISTS SOURCES)
    get_filename_component(EXT "${FILE_PATH}" EXT)
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/src" "${FILE_PATH}")

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

set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/Export")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXPORT_DIR}"
)

# Assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DSRC="${CMAKE_SOURCE_DIR}/assets"
        -DDST="${EXPORT_DIR}"
        -DMANIFEST="${CMAKE_BINARY_DIR}/asset_manifest.txt"
        -P "${CMAKE_SOURCE_DIR}/cmake/sync_assets.cmake"
    COMMENT "Syncing assets..."
)
# DLL

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/x64")
else()
    set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/x86")
endif()

set(DLLS_TO_COPY
    bass.dll
    avcodec-61.dll
    avdevice-61.dll
    avformat-61.dll
    avutil-59.dll
    swresample-5.dll
    swscale-8.dll
)

foreach(DLL ${DLLS_TO_COPY})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${THIRDPARTY_DIR}/${DLL}"
            "${EXPORT_DIR}"
    )
endforeach()

if(MSVC)
    # set source and execution charset to utf-8
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
