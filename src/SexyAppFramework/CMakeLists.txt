# CmakeLists.txt
project(SexyAppFramework)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/SexyAppFramework/*.c"
    "${CMAKE_SOURCE_DIR}/src/SexyAppFramework/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/SexyAppFramework/*.h"
)

# popcap...WHYYYY
set(EXCLUDE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangleInc1.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_DrawTriangleInc2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SWTri_GetTexel.cpp"
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

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _LIB
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../dx8sdk/include"
)

foreach(FILE_PATH IN LISTS SOURCES)
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/src/SexyAppFramework" "${FILE_PATH}")
    get_filename_component(FOLDER_PATH "${REL_PATH}" PATH)
    string(REPLACE "/" "\\" FILTER_PATH "${FOLDER_PATH}")
    source_group("${FILTER_PATH}" FILES "${FILE_PATH}")
endforeach()
