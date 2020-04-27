target_sources(gui
    PRIVATE
    Application.cpp
    Color.cpp
    Native.cpp
    Shape.cpp
    Transform.cpp
    Transformable.cpp
    imgui_custom.cpp
    nanovg_custom.cpp
)

add_subdirectory(Fonts)