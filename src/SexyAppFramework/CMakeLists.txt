# CmakeLists.txt
project(SexyAppFramework)

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

# -- Library Setup Start

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

set(LIBTYPE "STATIC" CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)

# -- Library Setup End

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL3::SDL3-static
    zlibstatic
    OpenAL::OpenAL
    glm
    ogg
    freetype
    Vorbis::vorbisfile
    Vorbis::vorbis
    ImageLib
    PakLib
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/x64/bass.lib)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/x86/bass.lib)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB

    # SAF specific
    USE_OGG_LIB
    SEXY_USE_OPENGL
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

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/thirdparty/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx8sdk/include"
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

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    install(FILES ${CMAKE_SOURCE_DIR}/thirdparty/x64/bass.dll DESTINATION "${CMAKE_SOURCE_DIR}/Dist")
else ()
   install(FILES ${CMAKE_SOURCE_DIR}/thirdparty/x86/bass.dll DESTINATION "${CMAKE_SOURCE_DIR}/Dist")
endif()
