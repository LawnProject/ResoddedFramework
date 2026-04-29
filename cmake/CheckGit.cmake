find_package(Git)

if(GIT_EXECUTABLE)
    message(STATUS "Git found, parsing Git information")

    get_filename_component(WORKING_DIR ${SRC} DIRECTORY)
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
message(STATUS "Git was not found, disabling Git information")
set(GIT_IS_DIRTY 0)
set(GIT_HASH none)
set(GIT_AVAILABLE 0)
endif()


configure_file(${SRC} ${DST} @ONLY)
