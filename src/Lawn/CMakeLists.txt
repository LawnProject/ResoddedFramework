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
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}../ImageLib"
    "${CMAKE_CURRENT_SOURCE_DIR}../PakLib"
    "${CMAKE_CURRENT_SOURCE_DIR}../Sexy.TodLib"
    "${CMAKE_CURRENT_SOURCE_DIR}../SexyAppFramework"
    "${CMAKE_CURRENT_SOURCE_DIR}../zlib"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    SexyAppFramework
    ImageLib
    PakLib
    zlibstatic
)

# Okay. Yes. We get it.
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 winmm)
endif()

foreach(FILE_PATH IN LISTS SOURCES)
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/src/Lawn" "${FILE_PATH}")
    get_filename_component(FOLDER_PATH "${REL_PATH}" PATH)
    string(REPLACE "/" "\\" FILTER_PATH "${FOLDER_PATH}")
    source_group("${FILTER_PATH}" FILES "${FILE_PATH}")
endforeach()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/Dist"
    CONFIGURATIONS Debug
    FILE_SET PDBS
    DESTINATION "${CMAKE_SOURCE_DIR}/Dist"
)
