macro(mahi_gui_example target)
    # create executable
    add_executable(${target} "ex_${target}.cpp" my_icon.rc)
    # set dependencies
    target_link_libraries(${target} mahi::gui)
    # add install rule
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    set_target_properties(${target} PROPERTIES FOLDER "Examples")
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
endmacro(mahi_gui_example)

mahi_gui_example(imgui_demo)
mahi_gui_example(implot_demo)
mahi_gui_example(plots_benchmark)
mahi_gui_example(dpi)
mahi_gui_example(likert)
mahi_gui_example(events)
mahi_gui_example(hidden)
mahi_gui_example(log_window)
mahi_gui_example(opengl)
mahi_gui_example(gears)
mahi_gui_example(raw_glfw)
mahi_gui_example(shapes)
mahi_gui_example(transparent)

if (MAHI_GUI_NATIVE_DIAGS)
    mahi_gui_example(files)
    mahi_gui_example(svg)
endif()

mahi_gui_example(nvg_fbo)
mahi_gui_example(nvg_demo)
target_sources(nvg_demo PUBLIC "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/demo.c")
target_include_directories(nvg_demo PUBLIC "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example")
add_custom_command(TARGET nvg_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/images" $<TARGET_FILE_DIR:nvg_demo>/images)
add_custom_command(TARGET nvg_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/entypo.ttf" $<TARGET_FILE_DIR:nvg_demo>)
add_custom_command(TARGET nvg_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/NotoEmoji-Regular.ttf" $<TARGET_FILE_DIR:nvg_demo>)
add_custom_command(TARGET nvg_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/Roboto-Regular.ttf" $<TARGET_FILE_DIR:nvg_demo>)
add_custom_command(TARGET nvg_demo POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/3rdparty/nanovg/example/Roboto-Bold.ttf" $<TARGET_FILE_DIR:nvg_demo>)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang AND WIN32)
    # Clang on Windows won't compile coroutines
elseif(CMAKE_SYSTEM MATCHES "Linux")
    # Only GCC? Need to confirm
elseif(MAHI_UTIL_COROUTINES)
    mahi_gui_example(puzzometry)
    mahi_gui_example(coroutines)
    mahi_gui_example(coroutines_benchmark)
    mahi_gui_example(conway)
endif()
