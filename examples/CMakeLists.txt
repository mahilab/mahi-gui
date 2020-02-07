add_executable(demo demo.cpp my_icon.rc)
target_link_libraries(demo mahi::gui)

add_executable(drawing drawing.cpp my_icon.rc)
target_link_libraries(drawing mahi::gui)

add_executable(files files.cpp my_icon.rc)
target_link_libraries(files mahi::gui)

add_executable(plots plots.cpp my_icon.rc)
target_link_libraries(plots mahi::gui)

add_executable(likert likert.cpp my_icon.rc)
target_link_libraries(likert mahi::gui)

add_executable(events events.cpp my_icon.rc)
target_link_libraries(events mahi::gui)

add_executable(msd msd.cpp my_icon.rc)
target_link_libraries(msd mahi::gui)

add_executable(nvg nvg.cpp my_icon.rc)
target_link_libraries(nvg mahi::gui)

if (MAHI_GUI_COROUTINES)
add_executable(coroutines coroutines.cpp my_icon.rc)
target_link_libraries(coroutines mahi::gui)
endif()