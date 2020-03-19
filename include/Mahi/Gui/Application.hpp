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
    void set_window_title(const std::string& title);
    /// Set the window top-left position
    void set_window_pos(int xpos, int ypos);
    /// Get the window top-left position: auto [x,y] = get_window_pos()
    std::pair<int,int> get_window_pos() const;
    /// Set the window size
    void set_window_size(int width, int height);
    /// Get the window size: auto [w,h] = get_window_size();
    std::pair<int,int> get_window_size() const;
    /// Sets the limits that the user can size the window if resizable (pass -1 for no limit)
    void set_window_size_limits(int min_width, int min_height, int max_width, int max_height);
    /// Center the window on a monitor
    void center_window(int monitor = 0);
    /// Minimizes the window
    void minimize_window();
    /// Maximizes the window
    void maximize_window();
    /// Restores a minimized/maximized window
    void restore_window();
    /// Hides the window
    void hide_window();
    /// Shows the window
    void show_window();
    /// Force the window to be focused
    void focus_window();
    /// Notify the user by requesting window attention
    void request_window_attention();
    /// Enable/disable VSync
    void set_vsync(bool enabled);
    /// Sets a target framelimit in hertz and disables VSync (pass 0 for no limit) 
    void set_frame_limit(util::Frequency freq);

    /// Get the mouse position 
    std::pair<float,float> get_mouse_pos() const;

public:
    /// Emitted when the Window moves
    util::Event<void(int,int)> on_window_moved;
    /// Emitted when the Window is resized
    util::Event<void(int,int)> on_window_resized;
    /// Emitted right before the Window is closed; return false to cancel the close
    util::Event<bool(void), util::CollectorBooleanAnd> on_window_closed;
    /// Emitted when file(s) is dropped, passes list of filepaths
    util::Event<void(const std::vector<std::string>&)> on_file_drop;
    /// Emitted when there is an internal GLFW error (error code, description)
    static util::Event<void(int, const std::string&)> on_error;

public:
    /// Window background color
    Color background_color;

protected:
    /// Internal GLFW window handle, you can use glfwXXX functions with this
    GLFWwindow *window;
    /// Internal NanoVG context pointer
    NVGcontext* vg;

private:
    bool m_vsync;
    util::Time m_frame_time;

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