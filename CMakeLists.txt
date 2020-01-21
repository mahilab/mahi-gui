cmake_minimum_required(VERSION 3.0.0)
project(glfw_testing VERSION 0.1.0)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/glfw)

# GLAD
set(GLAD_SRC 
    3rdparty/glad/include/glad/glad.h
    3rdparty/glad/include/KHR/khrplatform.h
    3rdparty/glad/src/glad.c
)

set(GLAD_INCLUDE_DIR 3rdparty/glad/include)

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

# SRC
set(SRC 
    Application.hpp
    Application.cpp
)

set(IMGUI_INCLUDE_DIR 3rdparty/imgui)

add_executable(glfw_testing main.cpp ${SRC} ${GLAD_SRC} ${IMGUI_SRC})
target_link_libraries(glfw_testing glfw)
target_include_directories(glfw_testing PRIVATE ${GLAD_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR})
target_compile_definitions(glfw_testing PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)