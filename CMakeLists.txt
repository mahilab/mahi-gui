cmake_minimum_required(VERSION 3.13.0)

# create project
project(mahui VERSION 1.0.0)
# add modules
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
include(GNUInstallDirs)

# create target and add an alias
add_library(mahui "")
add_library(mahui::mahui ALIAS mahui)
set_target_properties(mahui PROPERTIES DEBUG_POSTFIX -d)
target_compile_features(mahui PUBLIC cxx_std_17 PRIVATE)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(mahui PUBLIC "/await" PRIVATE) # COROUTINES
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(mahui PUBLIC "-std=c++2a" "-fcoroutines-ts" "-stdlib=libc++") # COROUTINES
endif()

add_subdirectory(src/MAHUI)

# add include files
file(GLOB_RECURSE MAHUI_INCLUDE "include/*.hpp" "include/*.inl" "include/*.h")
target_sources(mahui PRIVATE ${MAHUI_INCLUDE})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/glfw)
target_link_libraries(mahui PUBLIC glfw)

# GLAD
set(GLAD_SRC 
    3rdparty/glad/include/glad/glad.h
    3rdparty/glad/include/KHR/khrplatform.h
    3rdparty/glad/src/glad.c
)
set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/include)
target_sources(mahui PRIVATE ${GLAD_SRC})

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
    3rdparty/imgui/imgui_impl_glfw.cpp
    3rdparty/imgui/imgui_impl_glfw.h
    3rdparty/imgui/imgui_impl_opengl3.cpp
    3rdparty/imgui/imgui_impl_opengl3.h
)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui)
target_sources(mahui PRIVATE ${IMGUI_SRC})

# set target properties
target_include_directories(mahui
	PUBLIC
		$<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${IMGUI_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP")    # MULTICORE BUILDS
endif()

# target_include_directories(mahui PUBLIC include ${GLAD_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR})
# add_subdirectory(src/MAHUI)

add_executable(example examples/example.cpp)
target_link_libraries(example mahui)