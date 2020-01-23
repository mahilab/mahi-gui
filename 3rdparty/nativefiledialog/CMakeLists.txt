if (WIN32)

set(NATIVE_FILE_DIALOG_SRC
    src/nfd_common.c
    src/nfd_common.h
    src/simple_exec.h
    src/common.h
    src/nfd_win
)

add_library(nativefiledialog STATIC ${NATIVE_FILE_DIALOG_SRC})
target_include_directories(nativefiledialog PUBLIC include)
target_link_libraries(nativefiledialog comctl32.lib)

endif()

