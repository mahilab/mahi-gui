#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotApi : public Application {
public:
    PlotApi() : Application(1000,1000,"Test") {
        X.resize(100); Y.resize(100);
        linspace(0, 1, X);
        linspace(0, 1, Y);
        for (int i = 0; i < Y.size(); i+=2)
            Y[i] += 0.25;
    }

    void update() {
        ImGui::Begin("Plot 1");
        if (ImGui::BeginPlot("My Plot 1##Derp")) {
            if (!ImGui::IsKeyDown(GLFW_KEY_1)) {
                ImGui::Plot("Data1", &X[0], &Y[0], 50, 0, 8);
            }
            if (!ImGui::IsKeyDown(GLFW_KEY_2)) {
                ImGui::Plot("Data2", &X[1], &Y[1], 50, 0, 8);
            }
            ImGui::EndPlot();
        }
        ImGui::End();

        ImGui::Begin("Plot 2");
        // if (ImGui::BeginPlot("Plot", &plt2, "Time", "Position")) {
        //     ImGui::Plot("Data", xs, ys, 4, ImPlotSpec_Asterisk, IM_COL_AUTO, {0,0,0,0});
        //     ImGui::PlotLabel("P[0]", xs[0], ys[0]);
        //     ImGui::EndPlot();
        // }
        ImGui::End();
    }

    float xs[4] = {0.1f,0.1f,0.4f,0.9f};
    float ys[4] = {0.9f,0.2f,0.6f,0.1f};
    std::vector<float> X, Y;
};

int main(int argc, char const *argv[])
{

    PlotApi api;
    api.run();
    return 0;
}
