// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

namespace ImGui {

void ShowImPlotDemoWindow(bool* p_open = NULL) {
    ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(main_viewport_pos.x + 650, main_viewport_pos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImPlot Demo", p_open);


    if (ImGui::CollapsingHeader("Help")) {
        ImGui::Text("USER GUIDE:");

           
    }

    if (ImGui::CollapsingHeader("Basic Plots")) {
        static float xs[11]  = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1};
        static float ys1[11] = {0.3, 0.6, 0.2, 0.8, 0.9, 1, 0, 0.8, 0.4, 0.1, 0.6};
        if (ImGui::BeginPlot("Lines and Points", NULL, NULL, {-1,300})) {
            ImGui::PlotBar("Bar", 0.05f, xs, ys1, 11);
            ImGui::Plot("Line", xs, ys1, 11);
            ImGui::EndPlot();
        }
    }

    if (ImGui::CollapsingHeader("Animated Plots")) {

    }


    if (ImGui::CollapsingHeader("Log Plots")) {
        static float xs[1000], ys[1000];
        for (int i = 0; i < 1000; ++i) {
            xs[i] = (float)(i*0.1f)+.1f;
            ys[i] = sin(xs[i]);
        }
        if (ImGui::BeginPlot("Log Plot")) {
            ImGui::Plot("Sin", xs, ys, 1000);
            ImGui::EndPlot();
        }
    }


    ImGui::End();

    ImGui::Begin("Style");
    ImGui::ShowStyleEditor();
    ImGui::End();
}

}

class PlotApi : public Application {
public:
    PlotApi() : Application() { }

    void update() {
        static bool p_open = true;
        ImGui::ShowImPlotDemoWindow(&p_open);
        if (!p_open)
            quit();
    }
};

int main(int argc, char const *argv[])
{

    PlotApi api;
    api.run();
    return 0;
}
