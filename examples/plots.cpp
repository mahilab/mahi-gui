#include <mahi/gui.hpp>

using namespace mahi::gui;

class PlotDemo : public Application {
public:
    PlotDemo() : Application(800,500,"Plot Demo") { 
        backgroundColor = Grays::Black;
        // ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
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
        plot.items[2].color = withAlpha(Reds::FireBrick, 0.5f);
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

    void DemoAxisControls(const char* label, ImGui::PlotAxis& axis) {
        ImGui::PushID(label);
        ImGui::Text(label); ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::DragFloatRange2("##Range", &axis.minimum, &axis.maximum, 0.01f, -10, 10); ImGui::SameLine();
        ImGui::SliderInt2("##Divisions", &axis.divisions, 0, 20);
        ImGui::PopItemWidth();
        ImGui::SameLine(); ImGui::Checkbox("##Grid", &axis.showGrid); 
        ImGui::SameLine(); ImGui::Checkbox("##Ticks", &axis.showTicks);
        ImGui::SameLine(); ImGui::Checkbox("##Labels", &axis.showLabels);
        ImGui::SameLine(); ImGui::Checkbox("##LockMin", &axis.lockMin);
        ImGui::SameLine(); ImGui::Checkbox("##LockMax", &axis.lockMax);
        ImGui::SameLine(); ImGui::Checkbox("##Flip", &axis.flip);
        ImGui::SameLine(); ImGui::ColorEdit4("Axis Color", (float*)&axis.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::PopID();
    }


    void update() override {
        Vec2 vpPos = ImGui::GetMainViewport()->Pos;
        Vec2 vpSize = ImGui::GetMainViewport()->Size;
        ImGui::SetNextWindowPos(vpPos + Vec2(5,5), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(vpSize - Vec2(10,10), ImGuiCond_Always);
        ImGui::Begin("Plots", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        DemoAxisControls("X-Axis",plot.xAxis);
        DemoAxisControls("Y-Axis",plot.yAxis);        
        ImGui::Text("      "); ImGui::SameLine(); ImGui::Checkbox("Crosshairs", &plot.showCrosshairs);
        ImGui::Separator();
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
