set(CLIPPER_SRC clipper.hpp clipper.cpp)
add_library(clipper ${CLIPPER_SRC})
target_include_directories(clipper PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:include>  
)