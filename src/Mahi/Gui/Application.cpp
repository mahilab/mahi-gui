#include <Mahi/Gui/Application.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5Brands.hpp>
#include <Mahi/Gui/imgui_custom.hpp>
#include <Mahi/Util/System.hpp>
#include <Mahi/Util/Timing/Clock.hpp>
#include "Fonts/Fonts.hpp"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "imgui_internal.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <thread>

#ifdef __linux__
#include <string.h>
#else
using std::memcpy;
#endif

using namespace mahi::util;

///////////////////////////////////////////////////////////////////////////////
// FORWARDS
///////////////////////////////////////////////////////////////////////////////

namespace mahi {
namespace gui {
    
namespace
{
// GLFW
static void glfw_context_version();
static void glfw_setup_window_callbacks(GLFWwindow* window, void* userPointer);
static void glfw_error_callback(int error, const char *description);
static void glfw_pos_callback(GLFWwindow* window, int xpos, int ypos);
static void glfw_size_callback(GLFWwindow* window, int width, int height);
static void glfw_close_callback(GLFWwindow* window);
static void glfw_key_callback(GLFWwindow*,int key, int scancode, int action, int mods);
static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths);
// IMGUI
static void configureImGui(GLFWwindow *window);
} // namespace

///////////////////////////////////////////////////////////////////////////////
// APPLICATION
///////////////////////////////////////////////////////////////////////////////

util::Event<void(int, const std::string&)> Application::on_error;

Application::Application(const Config& conf) : 
    m_window(nullptr), m_conf(conf), m_nvg(nullptr), m_frame_time(Time::Zero)
{
    // setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);
    // initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");
    // setup GLFW context version
    glfw_context_version();
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
        else
        {
            int count;
            auto monitors = glfwGetMonitors(&count);
            if (conf.monitor < count)
                monitor = monitors[conf.monitor];
            else
                monitor = glfwGetPrimaryMonitor();
        }
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (!mode)
            throw std::runtime_error("Failed to get Video Mode!");    
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        m_window = glfwCreateWindow(mode->width, mode->height, conf.title.c_str(), monitor, NULL);
    }
    else {
        m_window = glfwCreateWindow(conf.width, conf.height, conf.title.c_str(), NULL, NULL);
    }
    if (m_window == NULL)
        throw std::runtime_error("Failed to create Window!");
    // Make OpenGL context current
    glfwMakeContextCurrent(m_window);
    // Enabel VSync
    set_vsync(true);
    // center window
    if (conf.center && !conf.fullscreen)
        center_window(conf.monitor);
    // Setup GLFW callbacks
    glfw_setup_window_callbacks(m_window, this);
    // Initialize OpenGL loader
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
        throw std::runtime_error("Failed to initialize OpenGL loader!");
    // enable MSAA and depth testing in OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  
    // initialize NanoVG
    if (conf.nvg_aa)
        m_nvg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    else
        m_nvg = nvgCreateGL3(NVG_STENCIL_STROKES); // | NVG_DEBUG
    if (m_nvg == NULL)
        throw std::runtime_error("Failed to create NanoVG context!");
    // configure ImGui
    configureImGui(m_window);
}

Application::Application() :
    Application(Config({"", 100, 100, 0, false, true, false, true, false, false, 4, true, true, Grays::Black})) 
{}

Application::Application(const std::string &title, int monitor) :
    Application(Config({title, 0, 0, monitor, true, false, true, true, false, false, 4, true, true, Grays::Black})) 
{}

Application::Application(int width, int height, const std::string &title, bool resizable, int monitor) :
    Application(Config({title, width, height, monitor, false, resizable, true, true, false, true, 4, true, true, Grays::Black})) 
{}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    nvgDeleteGL3(m_nvg);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::run()
{
    ImGuiIO &io = ImGui::GetIO();
    util::Clock clock;
    util::Clock dt_clk;
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Clear frame, setup dendering
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);
        if (!m_conf.transparent)
            glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b, m_conf.background.a);
        else {
            if (m_conf.background.a != 1) // user wants a transparent fill
                glClearColor(m_conf.background.r, m_conf.background.g, m_conf.background.b, m_conf.background.a);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update
        m_dt = dt_clk.restart();
        update();
#ifdef MAHI_COROUTINES
        // resume coroutines
        if (!m_coroutines.empty())
        {
            std::vector<util::Enumerator> temp;
            temp.swap(m_coroutines);
            for (auto &coro : temp)
            {
                if (coro.move_next())
                    m_coroutines.push_back(std::move(coro));
            }
        }
#endif 
        // nanovg
        int winWidth, winHeight;
        glfwGetWindowSize(m_window, &winWidth, &winHeight);
        float pxRatio = static_cast<float>(fbWidth) / static_cast<float>(winWidth);
        nvgBeginFrame(m_nvg, static_cast<float>(winWidth), static_cast<float>(winHeight), pxRatio);
        draw(m_nvg);
        nvgEndFrame(m_nvg);
        // ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        if (!m_conf.vsync && m_frame_time != util::Time::Inf) {
            
            util::sleep(m_frame_time - clock.get_elapsed_time());       
            clock.restart();     
        }
        glfwSwapBuffers(m_window);
    }
    on_application_quit.emit();
}

void Application::quit() {
    glfwSetWindowShouldClose(m_window, 1);
}

util::Time Application::time() const {
    return util::seconds(glfwGetTime());
}

util::Time Application::dt() const {
    return m_dt;
}

void Application::set_background(const Color& color) {
    m_conf.background = color;
}

void Application::set_window_title(const std::string& title) {
    glfwSetWindowTitle(m_window, title.c_str());
}

void Application::set_window_pos(int xpos, int ypos) {
    glfwSetWindowPos(m_window, xpos, ypos);
}

Vec2 Application::get_window_pos() const {
    int xpos, ypos;
    glfwGetWindowPos(m_window, &xpos, &ypos);
    return {(float)xpos, (float)ypos};
}

void Application::set_window_size(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
}

Vec2 Application::get_window_size() const {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return {(float)width, (float)height};
}

void Application::set_window_size_limits(int min_width, int min_height, int max_width, int max_height) {
    glfwSetWindowSizeLimits(m_window, min_width, min_height, max_width, max_height);
}

void Application::center_window(int monitorIdx) {
    GLFWmonitor *monitor = nullptr;
    if (monitorIdx == 0) 
        monitor = glfwGetPrimaryMonitor();
    else
    {
        int count;
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
    glfwSetWindowPos(m_window, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);
}

void Application::minimize_window() {
    glfwIconifyWindow(m_window);
}

void Application::maximize_window() {
    glfwMaximizeWindow(m_window);
}

void Application::restore_window() {
    glfwRestoreWindow(m_window);
}

void Application::hide_window() {
    glfwHideWindow(m_window);
}

void Application::show_window() {
    glfwShowWindow(m_window);
}

void Application::focus_window() {
    glfwFocusWindow(m_window);
}

void Application::request_window_attention() {
    glfwRequestWindowAttention(m_window);
}

void Application::set_vsync(bool enabled) {
    m_conf.vsync = enabled;
    if (m_conf.vsync)
        glfwSwapInterval(1); // Enable vsync
    else
        glfwSwapInterval(0); // Disable vsync
}

void Application::set_frame_limit(util::Frequency freq) {
    set_vsync(false);
    m_frame_time = freq.to_time();
}

Vec2 Application::get_mouse_pos() const {
    double x,y;
    glfwGetCursorPos(m_window, &x, &y);
    return {(float)x,(float)y};
}


#ifdef MAHI_COROUTINES

std::shared_ptr<util::Coroutine> Application::start_coroutine(util::Enumerator &&e)
{
    auto h = e.get_coroutine();
    m_coroutines.push_back(std::move(e));
    return h;
}

void Application::stop_coroutine(std::shared_ptr<util::Coroutine> routine)
{
    if (routine)
        routine->stop();
}

void Application::stop_coroutines()
{
    m_coroutines.clear();
}

int Application::coroutine_count() const
{
    return static_cast<int>(m_coroutines.size());
}

#endif

namespace
{

///////////////////////////////////////////////////////////////////////////////
// GLFW
///////////////////////////////////////////////////////////////////////////////

static void glfw_context_version()
{
    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
}

void glfw_setup_window_callbacks(GLFWwindow* window, void* userPointer) {
    glfwSetWindowUserPointer(window, userPointer);
    glfwSetWindowPosCallback(window, glfw_pos_callback);
    glfwSetWindowSizeCallback(window, glfw_size_callback);
    glfwSetWindowCloseCallback(window, glfw_close_callback);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetDropCallback(window, glfw_drop_callback);
}

static void glfw_error_callback(int error, const char *description)
{
    static std::string dsc = description;
    Application::on_error.emit(error, dsc);
}

static void glfw_pos_callback(GLFWwindow* window, int xpos, int ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->on_window_moved.emit(xpos, ypos);
}

static void glfw_size_callback(GLFWwindow* window, int width, int height) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_window_resized.emit(width, height);
}

static void glfw_close_callback(GLFWwindow* window) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    auto close = app->on_window_closed.emit();
    if (!close) 
        glfwSetWindowShouldClose(window, GLFW_FALSE);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->on_keyboard.emit(key,scancode,action,mods);
}

static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths)
{
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
static void configureImGui(GLFWwindow *window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // we are doing 4X MSAA with GLFW
    // ImGui::GetStyle().AntiAliasedFill = false;
    // ImGui::GetStyle().AntiAliasedLines = false;
    
    // add fonts
    io.Fonts->Clear();
    ImFontConfig font_cfg;
    font_cfg.PixelSnapH = true;
    font_cfg.OversampleH = 1;
    font_cfg.OversampleV = 1;
    strcpy(font_cfg.Name, "Roboto Mono Bold");
    unsigned char *fontCopy1 = new unsigned char[RobotoMono_Bold_ttf_len];
    memcpy(fontCopy1, &RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy1, RobotoMono_Bold_ttf_len, 15.0f, &font_cfg);

    ImFontConfig icons_config;
    icons_config.MergeMode        = true;
    icons_config.PixelSnapH       = true;
    icons_config.GlyphMinAdvanceX = 14.0f;
    icons_config.GlyphOffset      = ImVec2(0, 0);
    icons_config.OversampleH      = 1;
    icons_config.OversampleV      = 1;

    // merge in icons from font awesome 5
    static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    unsigned char *fontCopy2 = new unsigned char[fa_solid_900_ttf_len];
    memcpy(fontCopy2, &fa_solid_900_ttf, fa_solid_900_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy2, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);

    // merge in icons from font awesome 5 brands
    static const ImWchar fab_ranges[] = {ICON_MIN_FAB, ICON_MAX_FAB, 0};
    unsigned char *fontCopy3 = new unsigned char[fa_brands_400_ttf_len];
    memcpy(fontCopy3, &fa_brands_400_ttf, fa_brands_400_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy3, fa_brands_400_ttf_len, 14, &icons_config, fab_ranges);   

    ImGuiStyle *imStyle = &ImGui::GetStyle();

    // Main
    imStyle->WindowPadding    = ImVec2(8, 8);
    imStyle->FramePadding     = ImVec2(3, 2);
    imStyle->ItemSpacing      = ImVec2(4, 4);
    imStyle->ItemInnerSpacing = ImVec2(4, 4);
    imStyle->IndentSpacing    = 20.0f;
    imStyle->ScrollbarSize    = 15.0f;
    imStyle->GrabMinSize      = 5.0f;

    // Rounding
    imStyle->WindowRounding    = 2.0f;
    imStyle->ChildRounding     = 2.0f;
    imStyle->FrameRounding     = 2.0f;
    imStyle->PopupRounding     = 2.0f;
    imStyle->ScrollbarRounding = 10.0f;
    imStyle->GrabRounding      = 2.0f;
    imStyle->TabRounding       = 2.0f;

    // Alignment
    imStyle->WindowMenuButtonPosition = ImGuiDir_Right;

    // Setup Dear ImGui style
    ImGui::StyleColorsMahiDark4();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

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
}

} // private namespace

} // namespace gui
} // namespace mahi
