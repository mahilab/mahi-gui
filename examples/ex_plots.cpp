#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotDemo : public Application
{
public:
    PlotDemo() : Application(800, 500, "Plot Demo")
    {
        // ImGui::DisableViewports();
        // setup
        plot.title = "My Plot";
        plot.xAxis.minimum = 0;
        plot.xAxis.maximum = 10;
        plot.xAxis.label = "My X-Axis";
        plot.yAxis.minimum = 0;
        plot.yAxis.maximum = 1;
        plot.yAxis.label = "My Y-Axis";
        items.resize(4);
        // Line
        items[0].label = "Line";
        items[0].type = ImGui::PlotItem::Line;
        items[0].color = Greens::Chartreuse;
        items[0].size = 1;
        items[0].data.resize(10000);
        for (int i = 0; i < 10000; ++i)
        {
            float x = i * 0.001f;
            float y = 0.5f + 0.5f * std::sin(2 * PI * 1 * x);
            items[0].data[i] = {x, y};
        }
        // Scatter
        items[1].label = "Scatter";
        items[1].type = ImGui::PlotItem::Scatter;
        items[1].color = Blues::DeepSkyBlue;
        items[1].size = 2;
        items[1].data.resize(100);
        for (int i = 0; i < 100; ++i)
        {
            float x = i * 0.1f;
            float y = 0.1f * x + random_range(-0.1f, 0.1f);
            items[1].data[i] = {x, y};
        }
        // XBar
        items[2].label = "X-Bar";
        items[2].type = ImGui::PlotItem::XBar;
        items[2].color = withAlpha(Reds::FireBrick, 0.5f);
        items[2].size = 0.5f;
        items[2].data.resize(10);
        for (int i = 0; i < 10; ++i)
            items[2].data[i] = {(float)i + 0.5f, (float)random_range(0.0, 1.0)};
        // YBar
        items[3].label = "Y-Bar";
        items[3].type = ImGui::PlotItem::YBar;
        items[3].color = Yellows::Gold;
        items[3].size = 0.05f;
        items[3].data.resize(10);
        for (int i = 0; i < 10; ++i)
            items[3].data[i] = {(float)random_range(0.0, 2.0), ((float)i + 0.5f) * 0.1f};

        ImGui::DisableViewports();
    }

    void DemoAxisControls(const char *label, ImGui::PlotAxis &axis)
    {
        ImGui::PushID(label);
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::DragFloatRange2("##Range", &axis.minimum, &axis.maximum, 0.01f, -10, 10);
        ImGui::HoverTooltip("Range", 0);
        ImGui::SameLine();
        ImGui::SliderInt2("##Divisions", &axis.divisions, 0, 20);
        ImGui::HoverTooltip("Divisions", 0);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Checkbox("##Grid", &axis.showGrid);
        ImGui::HoverTooltip("Show Grid", 0);
        ImGui::SameLine();
        ImGui::Checkbox("##Ticks", &axis.showTickMarks);
        ImGui::HoverTooltip("Show Ticks", 0);
        ImGui::SameLine();
        ImGui::Checkbox("##Labels", &axis.showTickLabels);
        ImGui::HoverTooltip("Show Labels", 0);
        ImGui::SameLine();
        ImGui::Checkbox("##LockMin", &axis.lockMin);
        ImGui::HoverTooltip("Lock Min", 0);
        ImGui::SameLine();
        ImGui::Checkbox("##LockMax", &axis.lockMax);
        ImGui::HoverTooltip("Lock Max", 0);
        ImGui::SameLine();
        ImGui::Checkbox("##Flip", &axis.flip);
        ImGui::HoverTooltip("Flip", 0);
        ImGui::SameLine();
        ImGui::ColorEdit4("Axis Color", (float *)&axis.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##Axis Label", &axis.label);
        ImGui::PopItemWidth();
        ImGui::PopID();
    }

    void update() override
    {
        Vec2 vpPos = ImGui::GetMainViewport()->Pos;
        Vec2 vpSize = ImGui::GetMainViewport()->Size;
        ImGui::BeginFixed("Plots", vpPos, vpSize, ImGuiWindowFlags_NoTitleBar);
        DemoAxisControls("X-Axis", plot.xAxis);
        DemoAxisControls("Y-Axis", plot.yAxis);
        ImGui::Text("      ");
        ImGui::SameLine();
        ImGui::Checkbox("Crosshairs", &plot.showCrosshairs);
        ImGui::SameLine();
        ImGui::Checkbox("Mouse Position", &plot.showMousePos);
        ImGui::SameLine();
        ImGui::Checkbox("Selection", &plot.enableSelection);
        ImGui::SameLine();
        ImGui::Checkbox("Controls", &plot.enableControls);
        ImGui::SameLine();
        ImGui::ColorEdit4("Frame Color", (float *)&plot.frameColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::ColorEdit4("Background Color", (float *)&plot.backgroundColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::ColorEdit4("Border Color", (float *)&plot.borderColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::ColorEdit4("Selection Color", (float *)&plot.selectionColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##Plot Title", &plot.title);
        ImGui::PopItemWidth();

        ImGui::Separator();
        ImGui::Checkbox("Line", &items[0].show);
        ImGui::SameLine();
        ImGui::Checkbox("Scatter", &items[1].show);
        ImGui::SameLine();
        ImGui::Checkbox("X Bar", &items[2].show);
        ImGui::SameLine();
        ImGui::Checkbox("Y Bar", &items[3].show);

        // plot (you want this)
        ImGui::Plot("My Plot", &plot, &items[0], items.size());
        ImGui::End();
    }
    ImGui::PlotInterface plot;
    std::vector<ImGui::PlotItem> items;
};

int main(int argc, char const *argv[])
{
    PlotDemo demo;
    demo.run();
    return 0;
}
