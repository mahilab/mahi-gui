#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotDraw : public Application {
public:
    PlotDraw() : Application(500,500,"Plots Drawing") { 
        ImGui::DisableViewports();
        item1.type = ImGui::PlotItem::Line;
        item1.size = 3;
        item2.type = ImGui::PlotItem::Scatter;
        item2.size = 5;
    };

    void update() override {

        ImGui::SetNextWindowPos({0,0}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({500,250}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Click the Plot Area##1");
        ImGui::Plot("draw",&plot1,&item1,1);
        if (ImGui::IsPlotHovered() && ImGui::GetIO().MouseClicked[0]) 
            item1.data.push_back(ImGui::GetPlotMousePos());        
        ImGui::End();

        ImGui::BeginFixed("Click the Plot Area##2",{0,250},{500,250});
        ImGui::Plot("draw",&plot2,&item2,1);
        if (ImGui::IsPlotHovered() && ImGui::GetIO().MouseClicked[0]) 
            item2.data.push_back(ImGui::GetPlotMousePos());        
        ImGui::End();

    }

    ImGui::PlotInterface plot1, plot2;
    ImGui::PlotItem item1, item2;
};

int main(int argc, char const *argv[])
{
    PlotDraw app;
    app.run();
    return 0;
}
