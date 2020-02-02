#pragma once
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

    /// Set the window title
    void setWindowTitle(const std::string& title);
    /// Set the window position
    void setWindowPos(int xpos, int ypos);
    /// Set the window size
    void setWindowSize(int width, int height);
    /// Center the window on a monitor
    void centerWindow(int monitor = 0);

public:
    /// Emitted when file(s) is dropped, passes list of filepaths
    Event<void(const std::vector<std::string> &)> onFileDrop;

public:
    /// Window background color
    Color backgroundColor;

protected:
    /// Internal GLFW window handle
    GLFWwindow *window;

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