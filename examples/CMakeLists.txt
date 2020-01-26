add_executable(demo demo.cpp my_icon.rc)
target_link_libraries(demo mahi::gui)

add_executable(drawing drawing.cpp my_icon.rc)
target_link_libraries(drawing mahi::gui)

add_executable(files files.cpp my_icon.rc)
target_link_libraries(files mahi::gui)

if (MAHI_GUI_COROUTINES)
add_executable(coroutines coroutines.cpp my_icon.rc)
target_link_libraries(coroutines mahi::gui)
endif()