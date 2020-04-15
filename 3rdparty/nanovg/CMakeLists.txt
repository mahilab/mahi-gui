set(NANOVG_SRC 
    src/fontstash.h
    src/nanovg_gl_utils.h
    src/nanovg_gl.h
    src/nanovg.h
    src/nanovg.c
    src/stb_image.h
    src/stb_truetype.h
)

add_library(nanovg ${NANOVG_SRC})
target_include_directories(nanovg PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  
    $<INSTALL_INTERFACE:include>  # <prefix>/include/mylib
)
target_compile_definitions(nanovg PUBLIC NANOVG_GLAD)

# add_executable(nanovg-demo example/demo.h example/demo.c example/example_gl3.c example/perf.c example/perf.h example/stb_image_write.h)
# target_link_libraries(nanovg-demo nanovg glfw glad)

# add_executable(nanovg-fbo  example/example_fbo.c example/perf.c example/perf.h example/stb_image_write.h)
# target_link_libraries(nanovg-fbo nanovg glfw glad)
