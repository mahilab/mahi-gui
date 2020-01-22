#pragma once
#include <memory>
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <mahi/Event.hpp>

namespace mahi::gui {

class Application {
public:
    /// Constructor
    Application(int width, int height, const char* title);  
    /// Destructor
    ~Application();
    /// Runs the application
    void run();
    /// Called once per frame
    virtual void update();    
public:
    Event<void(int,const char**)> onFileDrop;
public:
    ImVec4 backgroundColor;
protected:
    GLFWwindow* m_window;
};

}