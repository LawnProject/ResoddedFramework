# Generates a header that embeds gamecontrollerdb.txt as a raw string literal
# so the SDL game controller database is compiled into the executable.
#
# MSVC limits a single string literal to 16380 characters (C2026), so the
# content is split into adjacent literals which the compiler concatenates.

if(NOT DEFINED IN_FILE OR NOT DEFINED OUT_FILE)
    message(FATAL_ERROR "IN_FILE and OUT_FILE are required")
endif()

if(NOT EXISTS "${IN_FILE}")
    message(FATAL_ERROR "Input gamecontrollerdb file not found: ${IN_FILE}")
endif()

file(READ "${IN_FILE}" CONTENT)

set(CHUNK_SIZE 16000)
string(LENGTH "${CONTENT}" CONTENT_LEN)

set(LITERALS "")
set(IDX 0)
while(IDX LESS CONTENT_LEN)
    math(EXPR REMAIN "${CONTENT_LEN} - ${IDX}")
    if(REMAIN GREATER CHUNK_SIZE)
        set(CHUNK_LEN ${CHUNK_SIZE})
    else()
        set(CHUNK_LEN ${REMAIN})
    endif()

    string(SUBSTRING "${CONTENT}" ${IDX} ${CHUNK_LEN} CHUNK_TEXT)
    string(REPLACE "\\" "\\\\" CHUNK_TEXT "${CHUNK_TEXT}")
    string(REPLACE "\"" "\\\"" CHUNK_TEXT "${CHUNK_TEXT}")
    string(REPLACE "\r" "\\r" CHUNK_TEXT "${CHUNK_TEXT}")
    string(REPLACE "\n" "\\n" CHUNK_TEXT "${CHUNK_TEXT}")

    string(APPEND LITERALS "\"${CHUNK_TEXT}\"\n")

    math(EXPR IDX "${IDX} + ${CHUNK_LEN}")
endwhile()

set(HEADER
"// Generated file - do not edit.
// Embedded SDL game controller database (https://github.com/mdqinc/SDL_GameControllerDB).
#pragma once

namespace GamepadDB
{
	inline constexpr char kData[] =
${LITERALS};
}
")

file(WRITE "${OUT_FILE}" "${HEADER}")
