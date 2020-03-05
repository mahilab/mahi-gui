target_sources(gui
    PRIVATE
    Application.cpp
    Color.cpp
    System.cpp
    Svg.cpp
    imgui_custom.cpp
    imgui_plot.cpp
)

add_subdirectory(Fonts)