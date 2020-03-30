// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <Mahi/Gui/Color.hpp>
#include <Mahi/Gui/Vec2.hpp>
#include <Mahi/Util/Event.hpp>
#include <Mahi/Util/Timing/Time.hpp>
#include <Mahi/Util/Timing/Frequency.hpp>

#ifdef MAHI_COROUTINES
#include <Mahi/Gui/Coroutine.hpp>
#endif

namespace mahi {
namespace gui {

/// A Windowed Application
class Application
{
public:
    /// Application Configuration options (for advanced Application construction)
    struct Config {
        std::string title  = "mahi-gui"; ///< window title
        int width          = 640;        ///< window width in pixels
        int height         = 480;        ///< window height in pixels
        int monitor        = 0;          ///< monitor the window will be on
        bool fullscreen    = false;      ///< should the window be fullscreen?
        bool resizable     = true;       ///< should the window be resizable?
        bool visible       = true;       ///< should the window be visible?
        bool decorated     = true;       ///< should the window have a title bar, close button, etc.?
        bool transparent   = false;      ///< should the window area be transparent?
        bool center        = true;       ///< should the window be centered to the monitor?
        int msaa           = 4;          ///< multisample anti-aliasing level (0 = none, 2, 4, 8, etc.)
        bool nvg_aa        = true;       ///< should NanoVG use anti-aliasing?
        bool vsync         = true;       ///< should VSync be enabled?
        Color background   = {0,0,0,1};  ///< OpenGL clear color, i.e. background color
    };

    /// Hidden Main Window Constructor (for using ImGui windows exclusively)
    Application();
    /// Fullscreen Main Window Constructor
    Application(const std::string& title, int monitor = 0);
    /// Windowed Main Window Constructor
    Application(int width, int height, const std::string& title, bool resizable = true, int monitor = 0);
    /// Advanced Construct from Application::Config instance
    Application(const Config& conf);
    /// Destructor
    ~Application();

    /// Runs the application
    void run();
    /// Quits the application
    void quit();
    /// Called once per frame
    virtual void update() { /* nothing by default */ }
    /// NanoVG drawing context, called immediately after update()
    virtual void draw(NVGcontext* nvg) { /* nothing by default */ }
    /// Get the current time
    util::Time time() const;
    /// Gets the delta time between consecutive calls to update
    util::Time dt() const;

    /// Set the window background (i.e. clear) color (no effect if transparent)
    void set_background(const Color& color);
    /// Set the window title
    void set_window_title(const std::string& title);
    /// Set the window top-left position
    void set_window_pos(int xpos, int ypos);
    /// Get the window top-left position: auto [x,y] = get_window_pos()
    Vec2 get_window_pos() const;
    /// Set the window size
    void set_window_size(int width, int height);
    /// Get the window size: auto [w,h] = get_window_size();
    Vec2 get_window_size() const;
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
    Vec2 get_mouse_pos() const;

public:
    /// Emitted when the Window moves
    util::Event<void(int,int)> on_window_moved;
    /// Emitted when the Window is resized
    util::Event<void(int,int)> on_window_resized;
    /// Emitted right before the Window is closed; return false to cancel the close
    util::Event<bool(void), util::CollectorBooleanAnd> on_window_closed;
    /// Emitted when a key is pressed, repeated, or release.
    util::Event<void(int,int,int,int)> on_keyboard;
    /// Emitted when the application quits
    util::Event<void(void)> on_application_quit;
    /// Emitted when file(s) is dropped, passes list of filepaths
    util::Event<void(const std::vector<std::string>&)> on_file_drop;
    /// Emitted when there is an internal GLFW error (error code, description)
    static util::Event<void(int, const std::string&)> on_error;

protected:
    /// Internal GLFW window handle, you can use glfwXXX functions with this
    GLFWwindow *m_window;
    /// Internal NVG Context, you use use nvgXXX functions with this
    NVGcontext* m_nvg;

private:
    Config m_conf;
    util::Time m_frame_time;
    util::Time m_dt;

#ifdef MAHI_COROUTINES
public:
    /// Starts a coroutine
    std::shared_ptr<util::Coroutine> start_coroutine(util::Enumerator &&coro);
    /// Stops an already running coroutine
    void stop_coroutine(std::shared_ptr<util::Coroutine> coro);
    /// Stops all running coroutines
    void stop_coroutines();
    /// Returns the number of coroutines running
    int coroutine_count() const;
private:
    /// Vector of running coroutines
    std::vector<util::Enumerator> m_coroutines;
#endif
};

} // namespace gui
} // namespace mahi