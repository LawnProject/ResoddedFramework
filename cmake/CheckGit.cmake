find_package(Git)

if(GIT_EXECUTABLE)
  get_filename_component(WORKING_DIR ${SRC} DIRECTORY)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${WORKING_DIR}
    OUTPUT_VARIABLE GIT_HASH
    RESULT_VARIABLE ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff --quiet
    WORKING_DIRECTORY ${WORKING_DIR}
    RESULT_VARIABLE ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

if (ERROR_CODE EQUAL 1)
    set(GIT_IS_DIRTY 1)
else()
    set(GIT_IS_DIRTY 0)
endif()

if(GIT_TAG STREQUAL "")
    set(GIT_HASH "")
    set(GIT_AVAILABLE 0)
    message(WARNING "Failed to determine hash from Git.")
else()
    set(GIT_AVAILABLE 1)
endif()

configure_file(${SRC} ${DST} @ONLY)