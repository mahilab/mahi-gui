#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include <mahi/Event.hpp>
#include <mahi/Color.hpp>

namespace mahi::gui {

class Application {
public:
    /// Fullscreen Constructor
    Application(const char* title, int monitor = 0);
    /// Windowed Mode Constructor
    Application(int width, int height, const char* title, bool resizable = true);  
    /// Destructor
    ~Application();
    /// Runs the application
    void run();
    /// Called once per frame
    virtual void update();    
public:   
    /// Emitted when a file(s) is dropped
    Event<void(int,const char**)> onFileDrop;
public:
    Color backgroundColor;
protected:
    GLFWwindow* window;
};

}