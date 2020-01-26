#include <mahi/Application.hpp>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include "Fonts/Fonts.hpp"
#include <mahi/Icons/IconsFontAwesome5.hpp>
#include <mahi/Icons/IconsFontAwesome5Brands.hpp>

///////////////////////////////////////////////////////////////////////////////
// FORWARDS
///////////////////////////////////////////////////////////////////////////////

namespace mahi::gui
{
namespace
{
// GLFW
static void glfw_context_version();
static void glfw_error_callback(int error, const char *description);
static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths);
// IMGUI
static void configureImGui(GLFWwindow *window);
} // namespace

///////////////////////////////////////////////////////////////////////////////
// APPLICATION
///////////////////////////////////////////////////////////////////////////////

Application::Application(const char *title, int monitorIdx) : window(nullptr)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");
    GLFWmonitor *monitor = nullptr;
    if (monitorIdx == 0) {
        monitor = glfwGetPrimaryMonitor();
        std::cout << "using def mon" << std::endl;
    }
    else
    {
        int count;
        auto monitors = glfwGetMonitors(&count);
        if (monitorIdx < count)
            monitor = monitors[monitorIdx];
        else
            monitor = glfwGetPrimaryMonitor();
    }
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        throw std::runtime_error("Failed to get Video Mode!");
    glfw_context_version();
    window = glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
    if (window == NULL)
        throw std::runtime_error("Failed to create Window!");
    // Setup OpenGL context
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsyncs
    // Setup GLFW Callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetDropCallback(window, glfw_drop_callback);
    // Initialize OpenGL loader
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
        throw std::runtime_error("Failed to initialize OpenGL loader!");
    // Setup ImGui
    configureImGui(window);
}

Application::Application(int width, int height, const char *title, bool resizable) : window(nullptr)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");
    glfw_context_version();
    if (!resizable)
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("Failed to create Window!");
    // Setup OpenGL context
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsyncs
    // Center window
    auto monitor = glfwGetPrimaryMonitor();
    if (!monitor)
        throw std::runtime_error("Failed to get Monitor!");
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        throw std::runtime_error("Failed to get Video Mode!");
    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwSetWindowPos(window,
                     monitorX + (mode->width - windowWidth) / 2,
                     monitorY + (mode->height - windowHeight) / 2);
    // Setup GLFW Callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetDropCallback(window, glfw_drop_callback);
    // Initialize OpenGL loader
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
        throw std::runtime_error("Failed to initialize OpenGL loader!");
    // Setup ImGui
    configureImGui(window);
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::run()
{
    ImGuiIO &io = ImGui::GetIO();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // update
        update();
#ifdef MAHI_GUI_COROUTINES
        // resume coroutines
        if (!m_coroutines.empty())
        {
            std::vector<Enumerator> temp;
            temp.swap(m_coroutines);
            for (auto &coro : temp)
            {
                if (coro.moveNext())
                    m_coroutines.push_back(std::move(coro));
            }
        }
#endif 
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        glfwSwapBuffers(window);
    }
}

void Application::update()
{
    // do nothing by default (user implemented)
}

#ifdef MAHI_GUI_COROUTINES

std::shared_ptr<Coroutine> Application::startCoroutine(Enumerator &&e)
{
    auto h = e.getCoroutine();
    m_coroutines.push_back(std::move(e));
    return h;
}

void Application::stopCoroutine(std::shared_ptr<Coroutine> routine)
{
    if (routine)
        routine->stop();
}

void Application::stopCoroutines()
{
    m_coroutines.clear();
}

int Application::coroutineCount() const
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

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void glfw_drop_callback(GLFWwindow *window, int count, const char **paths)
{
    static std::vector<std::string> pathsVec;
    pathsVec.resize(count);
    for (int i = 0; i < count; ++i)
        pathsVec[i] = paths[i];
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->onFileDrop.emit(pathsVec);
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // add fonts
    io.Fonts->Clear();
    ImFontConfig font_cfg;
    strcpy(font_cfg.Name, "Roboto Mono Bold");
    unsigned char *fontCopy1 = new unsigned char[RobotoMono_Bold_ttf_len];
    std::memcpy(fontCopy1, &RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy1, RobotoMono_Bold_ttf_len, 15.0f, &font_cfg);

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = 14.0f;
    icons_config.GlyphOffset = ImVec2(0, 0);

    // merge in icons from font awesome 5
    static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    unsigned char *fontCopy2 = new unsigned char[fa_solid_900_ttf_len];
    std::memcpy(fontCopy2, &fa_solid_900_ttf, fa_solid_900_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy2, fa_solid_900_ttf_len, 14.0f, &icons_config, fa_ranges);

    // merge in icons from font awesome 5 brands
    static const ImWchar fab_ranges[] = {ICON_MIN_FAB, ICON_MAX_FAB, 0};
    unsigned char *fontCopy3 = new unsigned char[fa_brands_400_ttf_len];
    std::memcpy(fontCopy3, &fa_brands_400_ttf, fa_brands_400_ttf_len);
    io.Fonts->AddFontFromMemoryTTF(fontCopy3, fa_brands_400_ttf_len, 14, &icons_config, fab_ranges);

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    ImGuiStyle *imStyle = &ImGui::GetStyle();

    // Main
    imStyle->WindowPadding = ImVec2(8, 8);
    imStyle->FramePadding = ImVec2(3, 2);
    imStyle->ItemSpacing = ImVec2(4, 4);
    imStyle->ItemInnerSpacing = ImVec2(4, 4);
    imStyle->IndentSpacing = 20.0f;
    imStyle->ScrollbarSize = 15.0f;
    imStyle->GrabMinSize = 5.0f;

    // Rounding
    imStyle->WindowRounding = 2.0f;
    imStyle->ChildRounding = 2.0f;
    imStyle->FrameRounding = 2.0f;
    imStyle->PopupRounding = 2.0f;
    imStyle->ScrollbarRounding = 10.0f;
    imStyle->GrabRounding = 2.0f;
    imStyle->TabRounding = 2.0f;

    // Alignment
    imStyle->WindowMenuButtonPosition = ImGuiDir_Right;

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
    colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

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
}

} // namespace

} // namespace mahi::gui