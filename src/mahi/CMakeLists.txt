target_sources(mahi_gui
    PRIVATE
    Application.cpp
    Color.cpp
    Print.cpp
    Random.cpp
    System.cpp
    imgui_custom.cpp
)

if(MAHI_GUI_COROUTINES)
    target_sources(mahi_gui
        PRIVATE
        Coroutine.cpp
    )
endif()

add_subdirectory(Fonts)