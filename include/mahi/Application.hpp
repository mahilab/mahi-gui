#pragma once
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mahi/Event.hpp>
#include <mahi/Color.hpp>
#include <mahi/Coroutine.hpp>

namespace mahi::gui
{

class Application
{
public:
    /// Fullscreen Constructor
    Application(const char *title, int monitor = 0);
    /// Windowed Mode Constructor
    Application(int width, int height, const char *title, bool resizable = true);
    /// Destructor
    ~Application();

    /// Runs the application
    void run();
    /// Called once per frame
    virtual void update();

    /// Starts a coroutine
    std::shared_ptr<Coroutine> startCoroutine(Enumerator &&coro);
    /// Stops an already running coroutine
    void stopCoroutine(std::shared_ptr<Coroutine> coro);
    /// Stops all running coroutines
    void stopCoroutines();
    /// Returns the number of coroutines running
    int coroutineCount() const;

public:
    /// Emitted when file(s) is dropped, passes list of filepaths
    Event<void(const std::vector<std::string> &)> onFileDrop;

public:
    /// Window background color
    Color backgroundColor;

protected:
    /// Internal GLFW window handle
    GLFWwindow *window;

private:
    /// Vector of running coroutines
    std::vector<Enumerator> m_coroutines;
};

} // namespace mahi::gui