#include <mahi/gui.hpp>

using namespace mahi::gui;

class PlotDemo : public Application {
public:
    PlotDemo() : Application(800,500,"Plot Demo") { 
        backgroundColor = Grays::Black;
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        // setup
        plot.xAxis.minimum = 0;
        plot.xAxis.maximum = 10;
        plot.yAxis.minimum = 0;
        plot.yAxis.maximum = 1;
        plot.items.resize(4);
        // Line
        plot.items[0].type = ImGui::PlotItem::Line;
        plot.items[0].color = Greens::Chartreuse;
        plot.items[0].size = 1;
        plot.items[0].data.resize(10000);
        for (int i = 0; i < 10000; ++i) {
            float x = i * 0.001f;
            float y = 0.5f + 0.5f * std::sin(2*Math::PI * 1 * x);
            plot.items[0].data[i] = {x,y};
        }
        // Scatter
        plot.items[1].type = ImGui::PlotItem::Scatter;
        plot.items[1].color = Blues::DeepSkyBlue;
        plot.items[1].size = 2;
        plot.items[1].data.resize(100);
        for (int i = 0; i < 100; ++i) {
            float x = i * 0.1f;
            float y = 0.1f * x + Random::range(-0.1f, 0.1f);
            plot.items[1].data[i] = {x,y};
        }
        // XBar
        plot.items[2].type = ImGui::PlotItem::XBar;
        plot.items[2].color = Reds::FireBrick;
        plot.items[2].size = 0.5f;
        plot.items[2].data.resize(10);
        for (int i = 0; i < 10; ++i)
            plot.items[2].data[i] = {(float)i + 0.5f, Random::range(0.0f,1.0f)};
        // YBar
        plot.items[3].type = ImGui::PlotItem::YBar;
        plot.items[3].color = Yellows::Gold;
        plot.items[3].size = 0.05f;
        plot.items[3].data.resize(10);
        for (int i = 0; i < 10; ++i)
            plot.items[3].data[i] = {Random::range(0.0f,2.0f), ((float)i + 0.5f) * 0.1f};

    }

    void update() override {
        Vec2 vpPos = ImGui::GetMainViewport()->Pos;
        Vec2 vpSize = ImGui::GetMainViewport()->Size;
        ImGui::SetNextWindowPos(vpPos + Vec2(5,5), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(vpSize - Vec2(10,10), ImGuiCond_Always);
        ImGui::Begin("Plots", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

        // demo controls (you don't need this)

        ImGui::DragFloatRange2("##X-Axis", &plot.xAxis.minimum, &plot.xAxis.maximum, 0.01f, -10, 10);
        ImGui::SameLine(); ImGui::Checkbox("##ShowX", &plot.xAxis.show);
        ImGui::SameLine(); ImGui::ColorEdit4("X-Axis Color", (float*)&plot.xAxis.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SliderInt2("##X-Divisions", &plot.xAxis.divisions, 0, 20);

        ImGui::DragFloatRange2("##Y-Axis", &plot.yAxis.minimum, &plot.yAxis.maximum, 0.01f, -10, 10);
        ImGui::SameLine(); ImGui::Checkbox("##ShowY", &plot.yAxis.show);
        ImGui::SameLine(); ImGui::ColorEdit4("Y-Axis Color", (float*)&plot.yAxis.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SliderInt2("##Y-Divisions", &plot.yAxis.divisions, 0, 20);

        ImGui::Checkbox("Line", &plot.items[0].show); ImGui::SameLine();
        ImGui::Checkbox("Scatter", &plot.items[1].show); ImGui::SameLine();
        ImGui::Checkbox("X Bar", &plot.items[2].show); ImGui::SameLine();
        ImGui::Checkbox("Y Bar", &plot.items[3].show);

        // animate Line
        for (int i = 0; i < 10000; ++i) {
            float x = i * 0.001f;
            float y = 0.5f + 0.5f * std::sin(2*Math::PI * 1 * (x + ImGui::GetTime()));
            plot.items[0].data[i] = {x,y};
        }


        // plot (you want this)
        ImGui::Plot("My Plot", plot);

        ImGui::End();
    }
    ImGui::PlotInterface plot;
};


int main(int argc, char const *argv[])
{
    PlotDemo demo;
    demo.run();
    return 0;
}
