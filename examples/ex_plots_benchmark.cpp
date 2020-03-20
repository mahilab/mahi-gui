#define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotBench : public Application {
public:
    PlotBench() : Application(500,500,"Plots Benchmark",false) { 
        set_vsync(false);
        ImGui::DisableViewports();
        for (int i = 0; i < 100; ++i)
            add_line();
    };
    void update() override {
        static bool open = true;
        ImGui::BeginFixed("Plot Benchmark",{0,0},{500,500}, ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button("VSync On"))
            set_vsync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            set_vsync(false);
        ImGui::SameLine();
        if (ImGui::Button("Add Line"))
            add_line();
        ImGui::SameLine();
        ImGui::Checkbox("Anit-Aliased", &ImGui::GetStyle().AntiAliasedLines);
        ImGui::SameLine();
        ImGui::Checkbox("Render", &render);
        ImGui::Text("%d lines @ %.3f FPS", items.size(), ImGui::GetIO().Framerate);
        if (render)
            ImGui::Plot("plot", plot, items);
        ImGui::End();
        if (!open)
            quit();
    }

    void add_line() {
        ImGui::PlotItem item;
        item.data.reserve(1000);
        item.color = random_color();
        float y = (float)random_range(0.0,1.0);
        for (int i = 0; i < 500; ++i)
            item.data.push_back(ImVec2(i*0.001f, y + (float)random_range(-0.01,0.01)));
        items.emplace_back(std::move(item));
    }

    ImGui::PlotInterface plot;
    std::vector<ImGui::PlotItem> items;
    bool render = true;
};

int main(int argc, char const *argv[])
{
    PlotBench app;
    app.run();
    return 0;
}
