find_package(Git)

# Define your mod's version here.

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
string(TIMESTAMP DATE_BUILD "%Y/%m/%d %H:%M:%S")

if(GIT_EXECUTABLE)
    message(STATUS "Git found, parsing Git information")
    get_filename_component(WORKING_DIR ${SRC} DIRECTORY)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --is-inside-work-tree
        WORKING_DIRECTORY ${WORKING_DIR}
        OUTPUT_VARIABLE IS_GIT_REPO
        RESULT_VARIABLE GIT_REPO_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

if(GIT_REPO_RESULT EQUAL 0 AND IS_GIT_REPO STREQUAL "true")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${WORKING_DIR}
        OUTPUT_VARIABLE GIT_HASH
        RESULT_VARIABLE HASH_ERROR_CODE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} diff --quiet
        WORKING_DIRECTORY ${WORKING_DIR}
        RESULT_VARIABLE DIRTY_ERROR_CODE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (DIRTY_ERROR_CODE EQUAL 1)
        set(GIT_IS_DIRTY 1)
    else()
        set(GIT_IS_DIRTY 0)
    endif()

    if(GIT_HASH STREQUAL "")
        set(GIT_HASH "")
        set(GIT_AVAILABLE 0)
        message(WARNING "Failed to determine hash from Git. Error Code: ${HASH_ERROR_CODE}")
    else()
        set(GIT_AVAILABLE 1)
    endif()
    message(STATUS "GIT_IS_DIRTY = ${GIT_IS_DIRTY}")
    message(STATUS "GIT_HASH = ${GIT_HASH}")
    message(STATUS "GIT_AVAILABLE = ${GIT_AVAILABLE}")

else()
    message(STATUS "Repository was not found, disabling Git information")
    set(GIT_IS_DIRTY 0)
    set(GIT_HASH none)
    set(GIT_AVAILABLE 0)
endif()

else()
    message(STATUS "Git was not found, disabling Git information")
    set(GIT_IS_DIRTY 0)
    set(GIT_HASH none)
    set(GIT_AVAILABLE 0)
endif()

set(BUILD_CACHE_FILE "${SEXY_PROJECT_ROOT}/cmake/BuildNumberCache.txt")

IF(EXISTS "${BUILD_CACHE_FILE}")
    message(STATUS "Incrementing build number")
	file(READ "${BUILD_CACHE_FILE}" BUILD_NUMBER)
	math(EXPR BUILD_NUMBER "${BUILD_NUMBER}+1")
ELSE()
    message(STATUS "Incrementing build number cache not found, setting to 0")
	set(BUILD_NUMBER "0")
ENDIF()
set(BUILD_DATE "${DATE_BUILD}")

file(WRITE "${BUILD_CACHE_FILE}" "${BUILD_NUMBER}")
configure_file(${SRC} ${DST} @ONLY)

set(VERSION_FILE "${SEXY_PROJECT_ROOT}/LawnVersion.txt")
file(WRITE "${VERSION_FILE}" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "Version written to ${VERSION_FILE}: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")