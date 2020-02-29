cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

option(MAHI_GUI_EXAMPLES   "Turn ON to build mahi-gui example(s)"            ON)
option(MAHI_GUI_COROUTINES "Turn ON to build experimental coroutine support" ON)

#===============================================================================
# FRONT MATTER
#===============================================================================

# create project
project(mahi_gui VERSION 1.0.0)

# tell user they can't have shared version of MEL if they try
if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "MEL does not yet support shared libaries")
endif()

# add modules
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
include(GNUInstallDirs)

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# create target and add an alias
add_library(mahi_gui "")
add_library(mahi::gui ALIAS mahi_gui)
set_target_properties(mahi_gui PROPERTIES DEBUG_POSTFIX -d)
target_compile_features(mahi_gui PUBLIC cxx_std_17 PRIVATE)

# coroutines
if(MAHI_GUI_COROUTINES)
    target_compile_definitions(mahi_gui PUBLIC MAHI_GUI_COROUTINES)
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(mahi_gui PUBLIC "/await")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(mahi_gui PUBLIC "-fcoroutines-ts")
    endif()
endif()

# add source files
add_subdirectory(src/mahi/gui)

# add include files
file(GLOB_RECURSE MAHI_GUI_INCLUDE "include/*.hpp" "include/*.inl" "include/*.h")
target_sources(mahi_gui PRIVATE ${MAHI_GUI_INCLUDE})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/glfw)
target_link_libraries(mahi_gui PUBLIC glfw)

# GLAD
add_subdirectory(3rdparty/glad)
target_link_libraries(mahi_gui PUBLIC glad)

# IMGUI
set(IMGUI_SRC
    3rdparty/imgui/imconfig.h
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_internal.h
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui.h
    3rdparty/imgui/imstb_rectpack.h
    3rdparty/imgui/imstb_textedit.h
    3rdparty/imgui/imstb_truetype.h    
    3rdparty/imgui/imgui_stdlib.h
    3rdparty/imgui/imgui_stdlib.cpp
    3rdparty/imgui/examples/imgui_impl_glfw.cpp
    3rdparty/imgui/examples/imgui_impl_glfw.h
    3rdparty/imgui/examples/imgui_impl_opengl3.cpp
    3rdparty/imgui/examples/imgui_impl_opengl3.h
)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui)
target_sources(mahi_gui PRIVATE ${IMGUI_SRC})

# NATIVE FILE DIALOG
add_subdirectory(3rdparty/nativefiledialog)
target_link_libraries(mahi_gui PUBLIC nativefiledialog)

# JSON
set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/json/include)

# NANOVG
add_subdirectory(3rdparty/nanovg)
target_link_libraries(mahi_gui PUBLIC nanovg)

# NANOSVG
set(NANOSVG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nanosvg/src)

# platform specific
if (WIN32)
target_link_libraries(mahi_gui PUBLIC Pdh Version)
endif()

# set target properties
target_include_directories(mahi_gui
	PUBLIC
		$<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${IMGUI_INCLUDE_DIR}
        ${JSON_INCLUDE_DIR}
        ${NANOSVG_INCLUDE_DIR}
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP")    # MULTICORE BUILDS
endif()

if (MAHI_GUI_EXAMPLES)
    add_subdirectory(examples)
endif()