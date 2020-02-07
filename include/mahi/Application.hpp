#pragma once
#include <memory>
#include <tuple>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <mahi/Event.hpp>
#include <mahi/Color.hpp>
#ifdef MAHI_GUI_COROUTINES
#include <mahi/Coroutine.hpp>
#endif

namespace mahi::gui
{

class Application
{
public:
    /// Fullscreen Constructor
    Application(const std::string& title = "", int monitor = 0);
    /// Windowed Mode Constructor
    Application(int width, int height, const std::string& title = "", bool resizable = true, int monitor = 0);
    /// Destructor
    ~Application();

    /// Runs the application
    void run();
    /// Quit the application
    void quit();
    /// Called once per frame
    virtual void update();

    /// Get the current time
    double time() const;

    /// Set the window title
    void setWindowTitle(const std::string& title);
    /// Set the window top-left position
    void setWindowPos(int xpos, int ypos);
    /// Get the window top-left position: auto [x,y] = getWindowPos()
    std::pair<int,int> getWindowPos() const;
    /// Set the window size
    void setWindowSize(int width, int height);
    /// Get the window size: auto [w,h] = getWindowSize();
    std::pair<int,int> getWindowSize() const;
    /// Sets the limits that the user can size the window if resizable (pass -1 for no limit)
    void setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight);
    /// Center the window on a monitor
    void centerWindow(int monitor = 0);
    /// Minimizes the window
    void minimizeWindow();
    /// Maximizes the window
    void maximizeWindow();
    /// Restores a minimized/maximized window
    void restoreWindow();
    /// Hides the window
    void hideWindow();
    /// Shows the window
    void showWindow();
    /// Force the window to be focused
    void focusWindow();
    /// Notify the user by requesting window attention
    void requestWindowAttention();
    /// Enable/disable VSync
    void enableVSync(bool enable);

    /// Get the mouse position 
    std::pair<float,float> getMousePosition() const;

public:
    /// Emitted when the Window moves
    Event<void(int,int)> onWindowMoved;
    /// Emitted when the Window is resized
    Event<void(int,int)> onWindowResized;
    /// Emitted right before the Window is closed; return false to cancel the close
    Event<bool(void), CollectorBooleanAnd> onWindowClosed;
    /// Emitted when file(s) is dropped, passes list of filepaths
    Event<void(const std::vector<std::string>&)> onFileDrop;
    /// Emitted when there is an internal GLFW error (error code, description)
    static Event<void(int, const std::string&)> onError;

public:
    /// Window background color
    Color backgroundColor;

protected:
    /// Internal GLFW window handle, you can use glfwXXX functions with this
    GLFWwindow *window;
    /// Internal NanoVG context pointer
    NVGcontext* vg;

#ifdef MAHI_GUI_COROUTINES
public:
    /// Starts a coroutine
    std::shared_ptr<Coroutine> startCoroutine(Enumerator &&coro);
    /// Stops an already running coroutine
    void stopCoroutine(std::shared_ptr<Coroutine> coro);
    /// Stops all running coroutines
    void stopCoroutines();
    /// Returns the number of coroutines running
    int coroutineCount() const;
private:
    /// Vector of running coroutines
    std::vector<Enumerator> m_coroutines;
#endif
};

} // namespace mahi::gui