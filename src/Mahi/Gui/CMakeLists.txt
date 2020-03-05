target_sources(gui
    PRIVATE
    Application.cpp
    Color.cpp
    Native.cpp
    Svg.cpp
    imgui_custom.cpp
    imgui_plot.cpp
)

add_subdirectory(Fonts)