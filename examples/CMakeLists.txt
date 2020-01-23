add_executable(demo demo.cpp my_icon.rc)
target_link_libraries(demo mahi::gui)

add_executable(drawing drawing.cpp my_icon.rc)
target_link_libraries(drawing mahi::gui)

add_executable(file_dialog file_dialog.cpp my_icon.rc)
target_link_libraries(file_dialog mahi::gui)