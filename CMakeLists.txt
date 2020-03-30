# mahi::gui CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Updated: 2/2020

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(MAHI_GUI_EXAMPLES   "Turn ON to build mahi-gui example(s)" ON)
else()
    option(MAHI_GUI_EXAMPLES   "Turn ON to build mahi-gui example(s)" OFF)
endif()

#===============================================================================
# FRONT MATTER
#===============================================================================

# create project
project(mahi-gui VERSION 1.0.0 LANGUAGES C CXX)

set(CXX_CMAKE_STANDARD 14)
set(CXX_CMAKE_STANDARD_REQUIRED ON)

# tell user they can't have shared version of MEL if they try
if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "mahi::gui does not support shared libaries")
endif()

# add ./cmake to CMake module path so our .cmake files can be found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs) # defines conventional GNU isntallation directories

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_SYSTEM MATCHES "Linux")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14") # ALL WARNINGS
    endif()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP")    # MULTICORE BUILDS
endif()

#===============================================================================
# 3RD PARTY DEPENDENCIES
#===============================================================================

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/glfw)
install(TARGETS glfw EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# GLAD
add_subdirectory(3rdparty/glad)
install(TARGETS glad EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# NATIVE FILE DIALOG
add_subdirectory(3rdparty/nativefiledialog-extended)
install(TARGETS nfd EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# IMGUI
set(IMGUI_HEADERS
    3rdparty/imgui/imconfig.h
    3rdparty/imgui/imgui_internal.h
    3rdparty/imgui/imgui.h
    3rdparty/imgui/imstb_rectpack.h
    3rdparty/imgui/imstb_textedit.h
    3rdparty/imgui/imstb_truetype.h    
    3rdparty/imgui/imgui_stdlib.h
)

set(IMGUI_SRC
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui_stdlib.cpp
    3rdparty/imgui/examples/imgui_impl_glfw.cpp
    3rdparty/imgui/examples/imgui_impl_glfw.h
    3rdparty/imgui/examples/imgui_impl_opengl3.cpp
    3rdparty/imgui/examples/imgui_impl_opengl3.h
)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui)

# NANOVG
add_subdirectory(3rdparty/nanovg)
install(TARGETS nanovg EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# NANOSVG
set(NANOSVG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nanosvg/src)

# CLIPPER
add_subdirectory(3rdparty/clipper)
install(TARGETS clipper EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#===============================================================================
# MAHI UTIL
#===============================================================================

include(FetchContent) 
FetchContent_Declare(mahi-util GIT_REPOSITORY https://github.com/mahilab/mahi-util.git) 
FetchContent_MakeAvailable(mahi-util)

#===============================================================================
# CREATE LIBRARY
#===============================================================================

# create target and add an alias
add_library(gui "")
add_library(mahi::gui ALIAS gui)
set_target_properties(gui PROPERTIES DEBUG_POSTFIX -d)
target_compile_features(gui PUBLIC cxx_std_17)
install(TARGETS gui EXPORT mahi-gui-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
set_target_properties(gui PROPERTIES OUTPUT_NAME "mahi-gui")

# defines
target_compile_definitions(gui PUBLIC MAHI_GUI IMGUI_IMPL_OPENGL_LOADER_GLAD)

# add source files
add_subdirectory(src/Mahi/Gui)

target_sources(gui PRIVATE ${IMGUI_SRC} ${IMGUI_HEADERS})

# add include files
file(GLOB_RECURSE MAHI_GUI_INCLUDE "include/*.hpp" "include/*.inl" "include/*.h")
target_sources(gui PRIVATE ${MAHI_GUI_INCLUDE}) # for intellisense 
target_include_directories(gui
	PUBLIC
		$<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${IMGUI_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NANOSVG_INCLUDE_DIR}>
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# link libraries
target_link_libraries(gui PUBLIC mahi::util)
target_link_libraries(gui PUBLIC glfw)
target_link_libraries(gui PUBLIC glad)
target_link_libraries(gui PUBLIC nanovg)
target_link_libraries(gui PUBLIC clipper)
target_link_libraries(gui PUBLIC nfd)
if(CMAKE_SYSTEM MATCHES "Linux")
    # https://techoverflow.net/2019/04/17/how-to-fix-gcc-undefined-reference-to-std-experimental-filesystem/
    target_link_libraries(gui PUBLIC GL stdc++fs)
endif()
#===============================================================================
# EXAMPLE EXECUTABLES
#===============================================================================

if (MAHI_GUI_EXAMPLES)
    add_subdirectory(examples)
endif()

#===============================================================================
# INSTALL
#===============================================================================

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MAHI_GUI_INSTALL_POSTFIX "") # 64 bit Windows is default
    else()
        set(MAHI_GUI_INSTALL_POSTFIX "-x86")
    endif()
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}${MAHI_GUI_INSTALL_POSTFIX}")

# install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY 3rdparty/glad/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${IMGUI_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES 3rdparty/nanovg/src/nanovg.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES 3rdparty/nanosvg/src/nanosvg.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES 3rdparty/clipper/clipper.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# set where we want to install our config
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/mahi-gui)

# export the targets to a script
install(EXPORT mahi-gui-targets
	FILE
		mahi-gui-targets.cmake
	NAMESPACE
		mahi::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

# include helper functions for creating config files that can be included by other projects to find and use a package
include(CMakePackageConfigHelpers)

# generate a package configuration file and a package version file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/mahi-gui-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/mahi-gui-config.cmake
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/mahi-gui-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

# install the config and configversion modules
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/mahi-gui-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/mahi-gui-config-version.cmake
	DESTINATION ${INSTALL_CONFIGDIR}
)

# export from the build tree
export(EXPORT mahi-gui-targets
	   NAMESPACE mahi::
	   FILE ${CMAKE_CURRENT_BINARY_DIR}/mahi-gui-targets.cmake)

