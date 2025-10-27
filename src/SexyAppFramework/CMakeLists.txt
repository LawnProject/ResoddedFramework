# CmakeLists.txt
project(SexyAppFramework)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})