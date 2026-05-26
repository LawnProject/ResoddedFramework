cmake_minimum_required(VERSION 3.3)

# Required for IN_LIST
cmake_policy(SET CMP0057 NEW)

set(IGNORE_EXTENSIONS
    ".exe"
    ".dll"
    ".pdb"
    ".lib"
    ".so"
    ".dylib"
)

# Collect all source asset files
file(GLOB_RECURSE SRC_FILES
    RELATIVE "${SRC}"
    "${SRC}/*"
)

# ============================================================
# Remove stale files
# ============================================================

if(EXISTS "${MANIFEST}")

    file(STRINGS "${MANIFEST}" OLD_FILES)

    foreach(F ${OLD_FILES})

        # File no longer exists in source assets
        if(NOT F IN_LIST SRC_FILES)

            get_filename_component(EXT "${F}" EXT)

            # Ignore binaries/libraries
            if(NOT EXT IN_LIST IGNORE_EXTENSIONS)

                if(EXISTS "${DST}/${F}")

                    file(REMOVE "${DST}/${F}")

                    message(STATUS
                        "Removed stale asset: ${F}"
                    )

                endif()

            endif()

        endif()

    endforeach()

endif()

# ============================================================
# Copy assets
# ============================================================

foreach(F ${SRC_FILES})

    get_filename_component(
        DEST_DIR
        "${DST}/${F}"
        DIRECTORY
    )

    file(MAKE_DIRECTORY "${DEST_DIR}")

    file(COPY_FILE
        "${SRC}/${F}"
        "${DST}/${F}"
        ONLY_IF_DIFFERENT
    )

endforeach()

# ============================================================
# Write manifest
# ============================================================

file(WRITE "${MANIFEST}" "")

foreach(F ${SRC_FILES})

    file(APPEND
        "${MANIFEST}"
        "${F}\n"
    )

endforeach()