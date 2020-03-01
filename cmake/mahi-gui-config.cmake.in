# if mahi::gui target has yet to be defined
if(NOT TARGET mahi::gui)
    # get directory mahi-daq-config.cmake was found in
    get_filename_component(MAHI_GUI_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # include find dependecny macro
    include(CMakeFindDependencyMacro)
    # find Threads (needed by GLFW apparently)
    find_package(Threads)
    # include the appropriate targets file
    include("${MAHI_GUI_CMAKE_DIR}/mahi-gui-targets.cmake")
endif()

# set mahi::gui libaries
set(MAHI_GUI_LIBRARIES mahi::gui)
