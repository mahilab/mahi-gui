## MAHUI

This grotesquely named library (short for MAHI UI), provides a basic foundation for making user interfaces and GUIs in C++. It includes [GLFW](https://www.glfw.org/) for creating Windows and OpenGL contexts, [Dear ImGui](https://github.com/ocornut/imgui) for all your GUI needs, and a few custom utility classes and functions to spice things up.

### Integration

The library is small and intended to be copied to your build tree, or better yet, added as a `git` [submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

Suppose you already have a CMake project under `git` source control:

```shell
my_project> git submodule add https://github.com/mahilab/MAHUI
```

The MAHUI directory will be cloned to the top level of `my_project`. Now, just add the following to your `CMakeLists.txt`:

```cmake
add_subdirectory(MAHUI)
add_executable(my_app "main.cpp")
target_link_libraries(my_app mahui)
```

That's it!