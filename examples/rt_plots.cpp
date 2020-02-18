#include <mahi/gui.hpp>

using namespace mahi::gui;

class RealTimePlotDemo : public Application {
public:
    RealTimePlotDemo() : Application(800,500,"Plot Demo") { 
        backgroundColor = Grays::Black;
        ImGui::DisableViewports();
        items1.resize(1);
        plot1.xAxis.maximum = 10;
        plot1.xAxis.lockMin = true;  plot1.xAxis.lockMax = true;
        items2 = items1;
        plot2 = plot1;
    }

    void update() override {
        ImGui::BeginFixed("Real Time Plots", {0,0}, {800,500}, ImGuiWindowFlags_NoTitleBar);     
        // rolling plot example
        ImGui::PlotItemRollPoint(items1[0], (float)time(), Random::range(0.49f,0.51f), 10.0f);
        ImGui::Plot("My Plot", plot1, items1, ImVec2(-1,240));
        // buffering plot example
        ImGui::PlotItemBufferPoint(items2[0], (float)time(), Random::range(0.49f,0.51f), 1000);
        plot2.xAxis.minimum = (float)time() - 10;
        plot2.xAxis.maximum = (float)time();
        ImGui::Plot("My Plot", plot2, items2, ImVec2(-1,-1));
        ImGui::End();
    }

    ImGui::PlotInterface plot1, plot2;
    std::vector<ImGui::PlotItem> items1, items2;
};

int main(int argc, char const *argv[])
{
    RealTimePlotDemo demo;
    demo.run();
    return 0;
}