target_sources(mahi_gui
    PRIVATE
    Application.cpp
    Color.cpp
    Print.cpp
    Random.cpp
    System.cpp
    ImGui.cpp
)

if(MAHI_GUI_COROUTINES)
    target_sources(mahi_gui
        PRIVATE
        Coroutine.cpp
    )
endif()

add_subdirectory(Fonts)