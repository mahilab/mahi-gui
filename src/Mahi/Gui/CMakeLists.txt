target_sources(gui
    PRIVATE
    Application.cpp
    Color.cpp
    Shape.cpp
    Transform.cpp
    Transformable.cpp
    imgui_custom.cpp
    nanovg_custom.cpp
)

if(MAHI_GUI_NATIVE_DIAGS)
    target_sources(gui PRIVATE Native.cpp)
endif()

add_subdirectory(Fonts)
