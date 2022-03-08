#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

namespace mahi {
namespace gui {

/// Constructor
Plotter::Plotter(PlotHelper& plot_helper) : Application(500,500,"Plotter"),
    m_plot_helper(plot_helper),
    m_stop(false)
{ 
    ImGui::DisableViewports();
    on_window_resized.connect(this, &Plotter::window_resize_handler); 
}

void Plotter::window_resize_handler(int width, int height) {
    render_imgui();
}

/// GUI code
void Plotter::update() {
    auto w_size = get_window_size();

    float t = static_cast<float>(time().as_seconds());
    ImGui::BeginFixed("Plotter", {0,0}, {w_size.x, w_size.y}, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    
    if (ImGui::Button(m_paused ? "Resume" : "Pause")){
        m_paused = !m_paused;
        m_paused_data = m_scrolling_data;
    } 

    auto plot_data = m_paused ? m_paused_data : m_scrolling_data;

    ImPlot::SetNextPlotLimitsX(t - 10, t, m_paused ? ImGuiCond_Once : ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsY(-2,2);
    if(ImPlot::BeginPlot("##Torque Plot", "Time (s)", "Data", {-1,-1}, 0, 0, 0)){
        for (const auto &data : plot_data){
            ImPlot::PlotLine((data.first).c_str(), &data.second.Data[0].x, &data.second.Data[0].y, data.second.Data.size(), data.second.Offset, 2 * sizeof(float));
        }
        ImPlot::EndPlot();
    }

    ImGui::End();


    auto new_data = m_plot_helper.get_data();
    for (auto& data_vector : new_data) {
        if (m_scrolling_data.find(data_vector.first) == m_scrolling_data.end()) {
            m_scrolling_data[data_vector.first] = ScrollingData();
        }
        for (auto& data_point: data_vector.second){
            m_scrolling_data[data_vector.first].AddPoint(data_point.first, static_cast<float>(data_point.second));
        }
    }

    if (!m_open || m_stop) {
        quit();
    }
}

void run_plotter(PlotHelper& plot_helper) {
    std::thread plotter_thread([&]() {
        mahi::gui::Plotter plotter(plot_helper);
        plotter.run();
    });
    plotter_thread.detach();
}

} // namespace gui
} // namespace mahi