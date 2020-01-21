#pragma once
#include <memory>
#include "imgui.h"
#include <GLFW/glfw3.h>

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
    ImVec4 backgroundColor;
protected:
    GLFWwindow* m_window;
};