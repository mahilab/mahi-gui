if (WIN32)

set(NATIVE_FILE_DIALOG_SRC
    src/nfd_common.c
    src/nfd_common.h
    src/simple_exec.h
    src/common.h
    src/nfd_win
)

add_library(nativefiledialog STATIC ${NATIVE_FILE_DIALOG_SRC})
target_include_directories(nativefiledialog PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)
target_link_libraries(nativefiledialog comctl32.lib)

elseif(APPLE)

find_library(APPKIT_LIBRARY AppKit)

set(NATIVE_FILE_DIALOG_SRC
    src/nfd_common.c
    src/nfd_common.h
    src/common.h
    src/nfd_cocoa.m
)

add_library(nativefiledialog STATIC ${NATIVE_FILE_DIALOG_SRC})
target_include_directories(nativefiledialog PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)
target_link_libraries(nativefiledialog PRIVATE ${APPKIT_LIBRARY})

endif()
