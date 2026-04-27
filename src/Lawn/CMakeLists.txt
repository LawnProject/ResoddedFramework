# CmakeLists.txt
project(LawnProject)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# ..and the lawn files in root dir
file(GLOB LAWN_FILES
    "${CMAKE_SOURCE_DIR}/src/*.rc"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"

    # Todlib is a part of pvz
    "${CMAKE_SOURCE_DIR}/src/Sexy.TodLib/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Sexy.TodLib/*.h"
)
list(APPEND SOURCES ${LAWN_FILES})

# popcap...WHYYYY
set(EXCLUDE_FILES
    "${CMAKE_SOURCE_DIR}/src/Sexy.TodLib/TodDrawTriangle.cpp"
    "${CMAKE_SOURCE_DIR}/src/Sexy.TodLib/TodDrawTriangleInc.cpp"
)

foreach(EXCLUDED_FILE IN LISTS EXCLUDE_FILES)
    list(REMOVE_ITEM SOURCES "${EXCLUDED_FILE}")
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB
    DO_FIX_BUGS
    _CRT_SECURE_NO_WARNINGS

    LAWN_DEBUG_TOOLS
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    SexyAppFramework
)

# Okay. Yes. We get it.
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm)
endif()

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

# Base PvZ Assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets/base"
        "${EXPORT_DIR}"
)

# Engine Assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets/framework"
        "${EXPORT_DIR}"
)
# Mod Assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets/mod"
        "${EXPORT_DIR}"
)

# DLL
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BASS_DLL "${CMAKE_SOURCE_DIR}/thirdparty/x64/bass.dll")
else()
    set(BASS_DLL "${CMAKE_SOURCE_DIR}/thirdparty/x86/bass.dll")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${BASS_DLL}"
        "${EXPORT_DIR}"
)