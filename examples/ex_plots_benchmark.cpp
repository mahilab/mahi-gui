#define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotBench : public Application {
public:
    PlotBench() : Application(500,500,"Plots Benchmark") { 
        set_vsync(false);
        ImGui::DisableViewports();
        for (int i = 0; i < 100; ++i)
        {
            ImGui::PlotItem item;
            item.data.reserve(1000);
            item.color = random_color();
            float y = i * 0.01f;
            for (int i = 0; i < 1000; ++i)
                item.data.push_back(ImVec2(i*0.001f, y + (float)random_range(-0.01,0.01)));
            items.emplace_back(std::move(item));
        }
    };
    void update() override {

        if (animate) {
            for (int i = 0; i < 100; ++i) {
                float y = i * 0.01f;
                for (int j = 0; j < 1000; ++j) {
                    items[i].data[j].x = j*0.001f;
                    items[i].data[j].y = y + (float)random_range(-0.01,0.01);
                }
            }
        }

        auto [w,h] = get_window_size();
        ImGui::BeginFixed("Plot Benchmark",{0,0},ImVec2(w,h), ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button("VSync On"))
            set_vsync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            set_vsync(false);
        ImGui::SameLine();
        ImGui::Checkbox("Anit-Aliased", &ImGui::GetStyle().AntiAliasedLines);
        ImGui::SameLine();
        ImGui::Checkbox("Render", &render);
        ImGui::SameLine();
        ImGui::Checkbox("Animate", &animate);

        ImGui::Text("%d lines @ %.3f FPS", items.size(), ImGui::GetIO().Framerate);
        if (render)
            ImGui::Plot("plot", plot, items);
        ImGui::End();
    }

    ImGui::PlotInterface plot;
    std::vector<ImGui::PlotItem> items;
    bool render = true;
    bool animate = true;
};

int main(int argc, char const *argv[])
{
    PlotBench app;
    app.run();
    return 0;
}
