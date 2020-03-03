target_sources(gui
    PRIVATE
    Application.cpp
    Color.cpp
    Print.cpp
    Random.cpp
    System.cpp
    Svg.cpp
    imgui_custom.cpp
    imgui_plot.cpp
)

if(MAHI_GUI_COROUTINES)
    target_sources(gui
        PRIVATE
        Coroutine.cpp
    )
endif()

add_subdirectory(Fonts)