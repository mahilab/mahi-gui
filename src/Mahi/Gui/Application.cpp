#include <Mahi/Gui/Application.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5Brands.hpp>
#include <Mahi/Gui/imgui_custom.hpp>
#include <Mahi/Util/System.hpp>
#include <Mahi/Util/Timing/Clock.hpp>
#include <Mahi/Gui/Fonts.hpp>

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <stdexcept>

#ifdef __linux__
#include <string.h>
#else
#include <cstring>
using std::memcpy;
#endif

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <ShellScalingAPI.h>
#endif

using namespace mahi::util;

///////////////////////////////////////////////////////////////////////////////
// FORWARDS
///////////////////////////////////////////////////////////////////////////////

namespace mahi {
namespace gui {

namespace {
// GLFW
static void glfw_context_version(bool gl_forward_compat);
static void glfw_setup_window_callbacks(GLFWwindow *window, void *userPointer);
static void glfw_error_callback(int error, const char *description);
static void glfw_pos_callback(GLFWwindow *window, int xpos, int ypos);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void glfw_close_callback(GLFWwindow *window);
static void glfw_key_callback(GLFWwindow *, int key, int scancode, int action, int mods);
static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths);
// IMGUI
static ImGuiContext* configureImGui(GLFWwindow *window, float dpi_scale);
}  // namespace

///////////////////////////////////////////////////////////////////////////////
// PLATFORM
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    void enable_dpi_aware() {
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        // const POINT ptZero = { 0, 0 };
        // auto monitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
        // UINT dpiX, dpiY;
        // auto result  = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        // return (float)dpiX / (float)USER_DEFAULT_SCREEN_DPI;
    }
#else
    void enable_dpi_aware() {
        // return 1.0f;
    }
#endif

///////////////////////////////////////////////////////////////////////////////
// APPLICATION
///////////////////////////////////////////////////////////////////////////////

util::Event<void(int, const std::string &)> Application::on_error;

Application::Application(const Config &conf) :
    m_window(nullptr),
    m_vg(nullptr),
    m_imgui_context(nullptr),
    m_implot_context(nullptr),
    m_conf(conf),
    m_frame_time(Time::Zero),
    m_dt(Time::Zero),
    m_time(Time::Zero),
    m_time_scale(1)
{
    // enable DPI awareness 
    if (m_conf.dpi_aware)
        enable_dpi_aware();
    float xscale, yscale;

    const char* err_msg;
    // setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);
    // initialize GLFW
    if (!glfwInit()) {
        glfwGetError(&err_msg);
        throw std::runtime_error(err_msg);
    }
    // setup GLFW context version
    glfw_context_version(conf.gl_forward_compat);
    // GLFW window hints
    glfwWindowHint(GLFW_RESIZABLE, conf.resizable);
    glfwWindowHint(GLFW_VISIBLE, conf.visible);
    glfwWindowHint(GLFW_DECORATED, conf.decorated);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, conf.transparent);
    glfwWindowHint(GLFW_SAMPLES, conf.msaa);
    // create GLFW window
    if (conf.fullscreen) {
        GLFWmonitor *monitor = nullptr;
        if (conf.monitor == 0)
            monitor = glfwGetPrimaryMonitor();
        else {
            int  count;
            auto monitors = glfwGetMonitors(&count);
            if (conf.monitor < count)
                monitor = monitors[conf.monitor];
            else
                monitor = glfwGetPrimaryMonitor();
        }
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (!mode) {
            glfwGetError(&err_msg);
            throw std::runtime_error(err_msg);
        }
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwGetMonitorContentScale(monitor,&xscale,&yscale);
        // glfwWindowHint(GLFW_AUTO_ICONIFY, false);
        m_window = glfwCreateWindow((int)(mode->width), (int)(mode->height), conf.title.c_str(), monitor, NULL);
    } 
    else {
        glfwGetMonitorContentScale(glfwGetPrimaryMonitor(),&xscale,&yscale);
        m_window = glfwCreateWindow((int)(conf.width * xscale), (int)(conf.height *yscale), conf.title.c_str(), NULL, NULL);
    }
    if (m_window == NULL) {
        glfwGetError(&err_msg);
        throw std::runtime_error(err_msg);
    }
    // Make OpenGL context current
    glfwMakeContextCurrent(m_window);
    // Enabel VSync
    set_vsync(conf.vsync);
    // center window
    if (conf.center && !conf.fullscreen)
        center_window(conf.monitor);
    // Setup GLFW callbacks
    glfw_setup_window_callbacks(m_window, this);
    // Initialize OpenGL loader
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
        throw std::runtime_error("Failed to initialize GLAD OpenGL loader!");
    // enable MSAA and depth testing in OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    // initialize NanoVG
    if (conf.nvg_aa)
        m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    else
        m_vg = nvgCreateGL3(NVG_STENCIL_STROKES);  // | NVG_DEBUG
    if (m_vg == NULL)
        throw std::runtime_error("Failed to create NanoVG context!");
    // configure ImGui
    m_imgui_context = configureImGui(m_window, xscale);
    if (!m_imgui_context)
        throw std::runtime_error("Failed to create ImGui context!");
    m_implot_context = ImPlot::CreateContext();
    if (!m_implot_context)
        throw std::runtime_error("Failed to create ImPlot context!");
    ImPlot::SetColormap(ImPlotColormap_Deep);
}

Application::Application() :
    Application(Config({"", 100, 100, 0, false, true, false, true, false, false, 4, true, true, true, true, Grays::Black})) {}

Application::Application(const std::string &title, int monitor) :
    Application(Config({title, 0, 0, monitor, true, false, true, true, false, false, 4, true, true, true, true, Grays::Black})) {}

Application::Application(int width, int height, const std::string &title, bool resizable, int monitor) :
    Application(Config({title, width, height, monitor, false, resizable, true, true, false, true, 4, true, true, true, true, Grays::Black})) {}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    if (m_implot_context) {
        ImPlot::DestroyContext(m_implot_context);
        m_implot_context = nullptr;
    }
    if (m_imgui_context) {
        ImGui::DestroyContext(m_imgui_context);
        m_imgui_context = nullptr;
    }
    if (m_vg) {
        nvgDeleteGL3(m_vg);
        m_vg = nullptr;
    }
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Application::render_imgui() {
    // ImGui::SetCurrentContext(m_imgui_context);
    ImGuiIO &   io = ImGui::GetIO();
    util::Clock clock;
    util::Clock dt_clk;

    Profile     prof;
    util::Clock prof_clk;
    glfwPollEvents();
    prof.t_poll = prof_clk.restart();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // main update
    prof_clk.restart();
    m_dt = dt_clk.restart() * m_time_scale;
    m_time += m_dt;
    update();
    prof.t_update = prof_clk.restart();

#ifdef MAHI_COROUTINES
    // resume coroutines
    if (!m_coroutines.empty()) {
        std::vector<util::Enumerator> temp;
        temp.swap(m_coroutines);
        for (auto &coro : temp) {
            if (coro.step())
                m_coroutines.push_back(std::move(coro));
        }
    }
    prof.t_coroutines = prof_clk.restart();
#endif

    // Clear frame, setup rendering
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    if (!m_conf.transparent)
        glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b,
                        m_conf.background.a);
    else {
        if (m_conf.background.a != 1)  // user wants a transparent fill
            glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b,
                            m_conf.background.a);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render OpenGL

    prof_clk.restart();
    draw();
    prof.t_gl = prof_clk.restart();

    // Render NanoVG

    int winWidth, winHeight;
    glfwGetWindowSize(m_window, &winWidth, &winHeight);
    float pxRatio = static_cast<float>(fbWidth) / static_cast<float>(winWidth);
    nvgBeginFrame(m_vg, static_cast<float>(winWidth), static_cast<float>(winHeight), pxRatio);
    draw(m_vg);
    nvgEndFrame(m_vg);
    prof.t_nvg = prof_clk.restart();

    // Render ImGui

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    prof.t_imgui = prof_clk.restart();

    // VSync or Idle

    if (!m_conf.vsync && m_frame_time != util::Time::Inf) {
        util::sleep(m_frame_time - clock.get_elapsed_time());
        clock.restart();
    }
    prof.t_idle = prof_clk.restart();

    // Swap OpenGL Buffers

    glfwSwapBuffers(m_window);
    prof.t_buffers = prof_clk.restart();

    // Set Profile

    m_profile = prof;
}

void Application::run() {
    ImGui::SetCurrentContext(m_imgui_context);
    ImGuiIO &   io = ImGui::GetIO();
    util::Clock clock;
    util::Clock dt_clk;
    while (!glfwWindowShouldClose(m_window)) {
        Profile     prof;
        util::Clock prof_clk;
        glfwPollEvents();
        prof.t_poll = prof_clk.restart();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // main update
        prof_clk.restart();
        m_dt = dt_clk.restart() * m_time_scale;
        m_time += m_dt;
        update();
        prof.t_update = prof_clk.restart();

#ifdef MAHI_COROUTINES
        // resume coroutines
        if (!m_coroutines.empty()) {
            std::vector<util::Enumerator> temp;
            temp.swap(m_coroutines);
            for (auto &coro : temp) {
                if (coro.step())
                    m_coroutines.push_back(std::move(coro));
            }
        }
        prof.t_coroutines = prof_clk.restart();
#endif

        // Clear frame, setup rendering
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);
        if (!m_conf.transparent)
            glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b,
                         m_conf.background.a);
        else {
            if (m_conf.background.a != 1)  // user wants a transparent fill
                glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b,
                             m_conf.background.a);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /// Render OpenGL

        prof_clk.restart();
        draw();
        prof.t_gl = prof_clk.restart();

        // Render NanoVG

        int winWidth, winHeight;
        glfwGetWindowSize(m_window, &winWidth, &winHeight);
        float pxRatio = static_cast<float>(fbWidth) / static_cast<float>(winWidth);
        nvgBeginFrame(m_vg, static_cast<float>(winWidth), static_cast<float>(winHeight), pxRatio);
        draw(m_vg);
        nvgEndFrame(m_vg);
        prof.t_nvg = prof_clk.restart();

        // Render ImGui

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        prof.t_imgui = prof_clk.restart();

        // VSync or Idle

        if (!m_conf.vsync && m_frame_time != util::Time::Inf) {
            util::sleep(m_frame_time - clock.get_elapsed_time());
            clock.restart();
        }
        prof.t_idle = prof_clk.restart();

        // Swap OpenGL Buffers

        glfwSwapBuffers(m_window);
        prof.t_buffers = prof_clk.restart();

        // Set Profile

        m_profile = prof;
    }
    on_application_quit.emit();
}

void Application::quit() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }

util::Time Application::real_time() const { return util::seconds(glfwGetTime()); }

util::Time Application::time() const { return m_time; }

util::Time Application::delta_time() const { return m_dt; }

void Application::set_time(util::Time t) { m_time = t; }

void Application::set_time_scale(float scale) { m_time_scale = scale; }

void Application::set_background(const Color &color) { m_conf.background = color; }

void Application::set_window_title(const std::string &title) {
    glfwSetWindowTitle(m_window, title.c_str());
}

void Application::set_window_pos(int xpos, int ypos) { glfwSetWindowPos(m_window, xpos, ypos); }

Vec2 Application::get_window_pos() const {
    int xpos, ypos;
    glfwGetWindowPos(m_window, &xpos, &ypos);
    return {(float)xpos, (float)ypos};
}

void Application::set_window_size(int width, int height) {
    float dpi_scale = get_dpi_scale();
    glfwSetWindowSize(m_window, width * dpi_scale, height * dpi_scale);
}

Vec2 Application::get_window_size() const {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return {(float)width, (float)height};
}

void Application::set_window_size_limits(int min_width, int min_height, int max_width,
                                         int max_height) {
    glfwSetWindowSizeLimits(m_window, min_width, min_height, max_width, max_height);
}

void Application::center_window(int monitorIdx) {
    GLFWmonitor *monitor = nullptr;
    if (monitorIdx == 0)
        monitor = glfwGetPrimaryMonitor();
    else {
        int  count;
        auto monitors = glfwGetMonitors(&count);
        if (monitorIdx < count)
            monitor = monitors[monitorIdx];
        else
            monitor = glfwGetPrimaryMonitor();
    }
    if (!monitor)
        throw std::runtime_error("Failed to get Monitor!");
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        throw std::runtime_error("Failed to get Video Mode!");
    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
    glfwSetWindowPos(m_window, monitorX + (mode->width - windowWidth) / 2,
                     monitorY + (mode->height - windowHeight) / 2);
}

void Application::minimize_window() { glfwIconifyWindow(m_window); }

void Application::maximize_window() { glfwMaximizeWindow(m_window); }

void Application::restore_window() { glfwRestoreWindow(m_window); }

void Application::hide_window() { glfwHideWindow(m_window); }

void Application::show_window() { glfwShowWindow(m_window); }

void Application::focus_window() { glfwFocusWindow(m_window); }

void Application::request_window_attention() { glfwRequestWindowAttention(m_window); }

Vec2 Application::get_framebuffer_size() const {
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    return Vec2((float)w, (float)h);
}

float Application::get_pixel_ratio() const {
    return get_framebuffer_size().x / get_window_size().x;
}

float Application::get_dpi_scale() const {    
    auto monitor = glfwGetPrimaryMonitor();
    float xscale = 1, yscale = 1;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    return xscale;
}

void Application::set_vsync(bool enabled) {
    m_conf.vsync = enabled;
    if (m_conf.vsync)
        glfwSwapInterval(1);  // Enable vsync
    else
        glfwSwapInterval(0);  // Disable vsync
}

void Application::set_frame_limit(util::Frequency freq) {
    set_vsync(false);
    m_frame_time = freq.to_time();
}

Vec2 Application::get_mouse_pos() const {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return {(float)x, (float)y};
}

const Application::Config& Application::get_config() const {
    return m_conf;
}

#ifdef MAHI_COROUTINES

std::shared_ptr<util::Coroutine> Application::start_coroutine(util::Enumerator &&e) {
    auto h = e.get_coroutine();
    m_coroutines.push_back(std::move(e));
    return h;
}

void Application::stop_coroutine(std::shared_ptr<util::Coroutine> routine) {
    if (routine)
        routine->stop();
}

void Application::stop_coroutines() { m_coroutines.clear(); }

int Application::coroutine_count() const { return static_cast<int>(m_coroutines.size()); }

YieldTimeScaled::YieldTimeScaled(util::Time duration, Application *app) :
    m_end(app->time() + duration), m_app(app) {}

bool YieldTimeScaled::is_over() { return m_app->time() >= m_end; }

std::shared_ptr<YieldTimeScaled> Application::yield_time_scaled(util::Time duration) {
    return ::mahi::gui::yield_time_scaled(duration, this);
}

#endif

const Application::Profile &Application::profile() const { return m_profile; }

namespace {

///////////////////////////////////////////////////////////////////////////////
// GLFW
///////////////////////////////////////////////////////////////////////////////

static void glfw_context_version(bool gl_forward_compat) {
    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    if (gl_forward_compat) {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
    }
#endif
}

void glfw_setup_window_callbacks(GLFWwindow *window, void *userPointer) {
    glfwSetWindowUserPointer(window, userPointer);
    glfwSetWindowPosCallback(window, glfw_pos_callback);
    glfwSetWindowSizeCallback(window, glfw_size_callback);
    glfwSetWindowCloseCallback(window, glfw_close_callback);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetDropCallback(window, glfw_drop_callback);
}

static void glfw_error_callback(int error, const char *description) {
    static std::string dsc = description;
    Application::on_error.emit(error, dsc);
}

static void glfw_pos_callback(GLFWwindow *window, int xpos, int ypos) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_window_moved.emit(xpos, ypos);
}

static void glfw_size_callback(GLFWwindow *window, int width, int height) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_window_resized.emit(width, height);
}

static void glfw_close_callback(GLFWwindow *window) {
    Application *app   = static_cast<Application *>(glfwGetWindowUserPointer(window));
    auto         close = app->on_window_closing.emit();
    if (!close)
        glfwSetWindowShouldClose(window, GLFW_FALSE);
}

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_keyboard.emit(key, scancode, action, mods);
}

static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths) {
    static std::vector<std::string> pathsVec;
    pathsVec.resize(count);
    for (int i = 0; i < count; ++i)
        pathsVec[i] = paths[i];
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_file_drop.emit(pathsVec);
}

///////////////////////////////////////////////////////////////////////////////
// IMGUI
///////////////////////////////////////////////////////////////////////////////
static ImGuiContext* configureImGui(GLFWwindow *window, float dpi_scale) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // we are doing 4X MSAA with GLFW
    // ImGui::GetStyle().AntiAliasedFill = false;
    // ImGui::GetStyle().AntiAliasedLines = false;

    // add fonts
    io.Fonts->Clear();
    ImFontConfig font_cfg;
    font_cfg.PixelSnapH           = true;
    font_cfg.OversampleH          = 1;
    font_cfg.OversampleV          = 1;
    font_cfg.FontDataOwnedByAtlas = false;
    strcpy(font_cfg.Name, "Roboto Mono Bold");
    io.Fonts->AddFontFromMemoryTTF(RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_len, IM_ROUND(15.0f * dpi_scale), &font_cfg);

    ImFontConfig icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.GlyphMinAdvanceX     = 14.0f;
    icons_config.GlyphOffset          = ImVec2(0, 0);
    icons_config.OversampleH          = 1;
    icons_config.OversampleV          = 1;
    icons_config.FontDataOwnedByAtlas = false;

    // merge in icons from font awesome 5
    static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, IM_ROUND(14.0f * dpi_scale), &icons_config,
                                   fa_ranges);

    // merge in icons from font awesome 5 brands
    static const ImWchar fab_ranges[] = {ICON_MIN_FAB, ICON_MAX_FAB, 0};
    io.Fonts->AddFontFromMemoryTTF(fa_brands_400_ttf, fa_brands_400_ttf_len, IM_ROUND(14.0f * dpi_scale), &icons_config,
                                   fab_ranges);

    ImGui::StyleColorsMahiDark4();
    ImGuiStyle &style = ImGui::GetStyle();
    // Main
    style.WindowPadding    = ImVec2(8, 8);
    style.FramePadding     = ImVec2(3, 2);
    style.ItemSpacing      = ImVec2(4, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing    = 20.0f;
    style.ScrollbarSize    = 15.0f;
    style.GrabMinSize      = 5.0f;
    // Rounding
    style.WindowRounding    = 2.0f;
    style.ChildRounding     = 2.0f;
    style.FrameRounding     = 2.0f;
    style.PopupRounding     = 2.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding      = 2.0f;
    style.TabRounding       = 2.0f;
    // Alignment
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    // Setup Dear ImGui style
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    // DPI scaling method 1 (this worked for my previous project, but not this...):
    io.FontGlobalScale = 1.0f / dpi_scale;
    io.DisplayFramebufferScale = ImVec2(dpi_scale,dpi_scale);
    // DPI scaling method 2:
    // style.ScaleAllSizes(dpi_scale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // // create device objects and set font  (Evan added this so it may break things)
    // ImGui_ImplOpenGL3_CreateDeviceObjects();
    // ImGui::SetCurrentFont(ImGui::GetDefaultFont());

    return context;
}

}  // namespace

}  // namespace gui
}  // namespace mahi
