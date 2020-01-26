<img src="https://raw.githubusercontent.com/mahilab/mahi-gui/master/mahi-gui.png" width="100"> 

## mahi-gui

This library provides a basic all-in-one foundation for making user interfaces and GUIs in C++. It bundles [GLFW](https://www.glfw.org/) and [glad](https://github.com/Dav1dde/glad) for creating Windows and OpenGL contexts, [Dear ImGui](https://github.com/ocornut/imgui) for all your GUI needs, and a few custom utility classes such as Coroutines and Events to spice things up. 

### Integration

The library is small and intended to be copied to your build tree, or better yet, added as a `git` [submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

Suppose you already have a CMake project under `git` source control:

```shell
> cd my_project
my_project> git submodule add https://github.com/mahilab/mahi-gui
```

The `mahi-gui` directory will be cloned to the top level of `my_project`. Now, just add the following to your `CMakeLists.txt`:

```cmake
set(MAHI_GUI_EXAMPLES OFF CACHE BOOL "" FORCE) # optional, ON by default
add_subdirectory(mahi-gui)
add_executable(my_app "my_app.cpp")
target_link_libraries(my_app mahi::gui)
```

That's it!

### Example Usage

```cpp
// my_app.cpp

#include <mahi/gui.hpp>
using namespace mahi::gui;

// Inherit from Application
class MyApp : public Application {
public:
    // 640x480 px window
    MyApp() : Application(640,480,"My App") { }
    // Override update (called once per frame)
    void update() override {
        // App logic and/or ImGui code goes here
        ImGui::Begin("Example");
        if (ImGui::Button("Herp"))
          print("Derp"); 
        ImGui::End();
    }
};

int main() {
    MyApp app;
    app.run();
    return 0;
}
```

Run and consult the examples for other features. Pay particular attention to [demo.cpp](https://github.com/mahilab/mahi-gui/blob/master/examples/demo.cpp) which shows all of the functionality of the **ImGui** library. It calls the `ImGui::ShowDemoWindow()` function from [imgui_demo.cpp](https://github.com/mahilab/mahi-gui/blob/master/3rdparty/imgui/imgui_demo.cpp), which itself is the absolute best place for **ImGui** examples. For a real-world example, see [Syntacts' GUI](https://github.com/mahilab/Syntacts/tree/master/gui/src), which is built entirely using **mahi gui**.

### Building on Windows


### Building on macOS

On macOS, we will use `LLVM clang` to build `mahi-gui`. While `Xcode` uses an Apple flavored version of  the `clang` compiler by default, the version you have installed my not be [up to date](https://en.wikipedia.org/wiki/Xcode#Version_comparison_table) with the required version of LLVM (> 8.0.0). Therefore, for this tutorial download the pre-built binaries for the latest version of LLVM from [here](http://releases.llvm.org/download.html).

```shell
> cd mahi-gui
> mkdir build
> cmake .. -DCMAKE_C_COMPILER="/path/to/clang/bin/clang" -DCMAKE_CXX_COMPILER="/path/to/clang/bin/clang++"
> cmake --build . --config Release
```

