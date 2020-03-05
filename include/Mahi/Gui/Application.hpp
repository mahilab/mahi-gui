#pragma once
#include <memory>
#include <tuple>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <Mahi/Gui/Color.hpp>
#include <Mahi/Util/Event.hpp>
#include <Mahi/Util/Timing/Time.hpp>
#include <Mahi/Util/Timing/Frequency.hpp>

#ifdef MAHI_COROUTINES
#include <Mahi/Util/Coroutine.hpp>
#endif

namespace mahi {
namespace gui {

/// A Windowed Application
class Application
{
public:

    /// Hidden Main Window Constructor (for using ImGui windows exclusively)
    Application();
    /// Fullscreen Main Window Constructor
    Application(const std::string& title, int monitor = 0);
    /// Windowed Main Window Constructor
    Application(int width, int height, const std::string& title, bool resizable = true, int monitor = 0);
    /// Destructor
    ~Application();

    /// Runs the application
    void run();
    /// Quitd the application
    void quit();
    /// Called once per frame
    virtual void update();

    /// Get the current time
    util::Time time() const;

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
    void setVSync(bool enabled);
    /// Sets a target framelimit in hertz and disables VSync (pass 0 for no limit) 
    void setFrameLimit(util::Frequency freq);

    /// Get the mouse position 
    std::pair<float,float> getMousePosition() const;

public:
    /// Emitted when the Window moves
    util::Event<void(int,int)> onWindowMoved;
    /// Emitted when the Window is resized
    util::Event<void(int,int)> onWindowResized;
    /// Emitted right before the Window is closed; return false to cancel the close
    util::Event<bool(void), util::CollectorBooleanAnd> onWindowClosed;
    /// Emitted when file(s) is dropped, passes list of filepaths
    util::Event<void(const std::vector<std::string>&)> onFileDrop;
    /// Emitted when there is an internal GLFW error (error code, description)
    static util::Event<void(int, const std::string&)> onError;

public:
    /// Window background color
    Color backgroundColor;

protected:
    /// Internal GLFW window handle, you can use glfwXXX functions with this
    GLFWwindow *window;
    /// Internal NanoVG context pointer
    NVGcontext* vg;

private:
    bool m_vsync;
    util::Time m_frameTime;

#ifdef MAHI_COROUTINES
public:
    /// Starts a coroutine
    std::shared_ptr<util::Coroutine> startCoroutine(util::Enumerator &&coro);
    /// Stops an already running coroutine
    void stopCoroutine(std::shared_ptr<util::Coroutine> coro);
    /// Stops all running coroutines
    void stopCoroutines();
    /// Returns the number of coroutines running
    int coroutineCount() const;
private:
    /// Vector of running coroutines
    std::vector<util::Enumerator> m_coroutines;
#endif
};

} // namespace gui
} // namespace mahi