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

struct ImGuiContext;
struct ImPlotContext;

namespace mahi {
namespace gui {

/// A Application
class Application {
public:
    /// Application Configuration options (for advanced Application construction)
    struct Config {
        std::string title      = "mahi-gui"; ///< window title
        int  width             = 640;        ///< window width in pixels
        int  height            = 480;        ///< window height in pixels
        int  monitor           = 0;          ///< monitor the window will be on
        bool fullscreen        = false;      ///< should the window be fullscreen?
        bool resizable         = true;       ///< should the window be resizable?
        bool visible           = true;       ///< should the window be visible?
        bool decorated         = true;       ///< should the window have a title bar, close button, etc.?
        bool transparent       = false;      ///< should the window area be transparent?
        bool center            = true;       ///< should the window be centered to the monitor?
        int  msaa              = 4;          ///< multisample anti-aliasing level (0 = none, 2, 4, 8, etc.)
        bool nvg_aa            = true;       ///< should NanoVG use anti-aliasing?
        bool vsync             = true;       ///< should VSync be enabled?
        bool dpi_aware         = false;      ///< does the application scale for high DPI? (WIP, DO NOT USE!!!)
        bool gl_forward_compat = true;       ///< should GLFW_OPENGL_FORWARD_COMPAT be set? Always set on Mac.
        Color background  = {0, 0, 0, 1};    ///< OpenGL clear color, i.e. window background color
    };

    /// Hidden Main Window Constructor (for using ImGui windows exclusively)
    Application();
    /// Fullscreen Main Window Constructor
    Application(const std::string& title, int monitor = 0);
    /// Windowed Main Window Constructor
    Application(int width, int height, const std::string& title, bool resizable = true, int monitor = 0);
    /// Advanced Constructor from Application::Config instance
    Application(const Config& conf);
    /// Destructor
    ~Application();

    /// Runs the application. Usually called from main.
    void run();
    /// Renders just the imgui portion. Mainly to use as a callback while resizing windows.
    void render_imgui();
    /// Quits the application. Usually called internally be derived classes.
    void quit();

    /// Get the current time (affected by time scale)
    util::Time time() const;
    /// Gets the delta time between consecutive calls to update (affected by time scale)
    util::Time delta_time() const;
    /// Gets the current real time since startup (not affected by time scale)
    util::Time real_time() const;
    /// Sets the time to a desired value
    void set_time(util::Time t);
    /// Sets the time scaling factor, which can be used for slow motion effects
    void set_time_scale(float scale);

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
    /// Returns the frame buffer size
    Vec2 get_framebuffer_size() const;
    /// Get pixel ratio (FB width / window width) for high DPI screens
    float get_pixel_ratio() const;
    /// Get the DPI scaling factor if enabled
    float get_dpi_scale() const;
    /// Enable/disable VSync
    void set_vsync(bool enabled);
    /// Sets a target framelimit in hertz and disables VSync (pass 0 for no limit)
    void set_frame_limit(util::Frequency freq);
   /// Get the mouse position
    Vec2 get_mouse_pos() const;

    /// Get the app's configuration
    const Config& get_config() const;

#ifdef MAHI_COROUTINES
    /// Starts a coroutine
    std::shared_ptr<util::Coroutine> start_coroutine(util::Enumerator&& coro);
    /// Stops an already running coroutine
    void stop_coroutine(std::shared_ptr<util::Coroutine> coro);
    /// Stops all running coroutines
    void stop_coroutines();
    /// Returns the number of coroutines running
    int coroutine_count() const;
    /// Yield instruction which waits for scaled time. Prefer this over mahi::util::yield_time.
    std::shared_ptr<YieldTimeScaled> yield_time_scaled(util::Time duration);
#endif

    /// Contains Application profiling information
    struct Profile {
        util::Time t_poll;        ///< time elapsed polling input events
        util::Time t_update;      ///< time elapsed across update()
        util::Time t_coroutines;  ///< time elapsed across all coroutines
        util::Time t_gl;          ///< time elapsed rendering raw OpenGL (i.e. inside of draw())
        util::Time t_nvg;      ///< time elapsed rendering NanoVG (i.e. inside of draw(NVGcontext*))
        util::Time t_imgui;    ///< time elapsed rendering ImGui
        util::Time t_idle;     ///< time elapsed idling
        util::Time t_buffers;  ///< time elapsed swapping OpenGL buffers
    };

    /// Gets the most recent profiling information
    const Profile& profile() const;

protected:
    /// Called once per frame. For application logic and ImGui. Do not make raw OpenGL calls here.
    virtual void update() { /* nothing by default */ }
    /// Generic OpenGL drawing context, called immediately after update().
    virtual void draw() { /* nothing by default */ }
    /// NanoVG specific drawing context, called immediately after draw()
    virtual void draw(NVGcontext* nvg) { /* nothing by default */ }

public:
    /// Emitted when the Window moves. Passes (x,y) window position pixels.
    util::Event<void(int, int)> on_window_moved;
    /// Emitted when the Window is resized. Passes (width,height) window size in pixels.
    util::Event<void(int, int)> on_window_resized;
    /// Emitted right before the Window is closed. Return false to cancel the close.
    util::Event<bool(void), util::CollectorBooleanAnd> on_window_closing;
    /// Emitted when a key is pressed, repeated, or release. Passes (key, scancode, action, mods). See GLFW.
    util::Event<void(int, int, int, int)> on_keyboard;
    /// Emitted when the application quits.
    util::Event<void(void)> on_application_quit;
    /// Emitted when file(s) is dropped Passes vector of filepaths.
    util::Event<void(const std::vector<std::string>&)> on_file_drop;
    /// Emitted when there is an internal GLFW error. Passes (error code, description).
    static util::Event<void(int, const std::string&)> on_error;

protected:
    /// Internal GLFW window handle, you can use glfwXXX functions with this
    GLFWwindow* m_window;
    /// Internal NVG Context, you can use use nvgXXX functions with this
    NVGcontext* m_vg;

private:
    ImGuiContext*  m_imgui_context;  ///< ImGui context of this application
    ImPlotContext* m_implot_context; ///< ImPlot context of this application
    Config         m_conf;           ///< Application configuration
    util::Time     m_frame_time;     ///< target time to sleep to each frame if VSync is disabled
    util::Time     m_dt;             ///< delta time (scaled)
    util::Time     m_time;           ///< Application time (scaled)
    float          m_time_scale;     ///< time scale (default = 1, no scale)
    Profile        m_profile;        ///< most recent Profile
#ifdef MAHI_COROUTINES
    std::vector<util::Enumerator> m_coroutines;  /// Vector of running coroutines
#endif
};

}  // namespace gui
}  // namespace mahi