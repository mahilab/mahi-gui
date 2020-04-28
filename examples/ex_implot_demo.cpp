// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotDemo : public Application {
public:
    PlotDemo() : Application() { 
        ImGui::StyleColorsDark();
    }
    void update() {
        static bool p_open = true;
        ImGui::ShowImPlotDemoWindow(&p_open);

        // ImGui::SetNextWindowSize(ImVec2(600,400));
        // ImGui::Begin("m", &p_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        // ImGui::End();

        if (!p_open)
            quit();
    }
};

int main(int argc, char const *argv[])
{
    PlotDemo demo;
    demo.run();
    return 0;
}
