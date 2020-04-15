#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotApi : public Application {

    using Application::Application;

    void update() {
        ImGui::Begin("Plot API");
        if (ImGui::BeginPlot("My Plot", &plt)) {
            
            ImGui::EndPlot();
        }
        ImGui::End();
    }

    ImGui::PlotInterface plt;
};

int main(int argc, char const *argv[])
{
    PlotApi api;
    api.run();
    return 0;
}
