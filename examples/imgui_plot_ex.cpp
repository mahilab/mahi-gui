// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include "imgui_plot.h"

using namespace mahi::gui;
using namespace mahi::util;

class PlotDemo : public Application {
public:
    PlotDemo() : Application() { 
        // ImGui::StyleColorsLight();
        generate_data();
    }
    void update() {
        static const float* y_data[] = { y_data1, y_data2, y_data3 };
        static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0), ImColor(0, 0, 255) };
        static uint32_t selection_start = 0, selection_length = 0;

        ImGui::Begin("Example plot", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        // Draw first plot with multiple sources
        ImGui::PlotConfig conf;
        conf.values.xs = x_data;
        conf.values.count = 512;
        conf.values.ys_list = y_data; // use ys_list to draw several lines simultaneously
        conf.values.ys_count = 3;
        conf.values.colors = colors;
        conf.scale.min = -1;
        conf.scale.max = 1;
        conf.tooltip.show = true;
        conf.grid_x.show = true;
        conf.grid_x.size = 128;
        conf.grid_x.subticks = 4;
        conf.grid_y.show = true;
        conf.grid_y.size = 0.5f;
        conf.grid_y.subticks = 5;
        conf.selection.show = true;
        conf.selection.start = &selection_start;
        conf.selection.length = &selection_length;
        conf.frame_size = ImVec2(512, 200);
        ImGui::Plot("plot1", conf);

        // Draw second plot with the selection
        // reset previous values
        conf.values.ys_list = nullptr;
        conf.selection.show = false;
        // set new ones
        conf.values.ys = y_data3;
        conf.values.offset = selection_start;
        conf.values.count = selection_length;
        conf.line_thickness = 2.f;
        ImGui::Plot("plot2", conf);

        ImGui::End();
    }

    void generate_data() {
        constexpr float sampling_freq = 44100;
        constexpr float freq = 500;
        for (size_t i = 0; i < 512; ++i) {
            const float t = i / sampling_freq;
            x_data[i] = t;
            const float arg = 2 * PI * freq * t;
            y_data1[i] = sin(arg);
            y_data2[i] = y_data1[i] * -0.6 + sin(2 * arg) * 0.4;
            y_data3[i] = y_data2[i] * -0.6 + sin(3 * arg) * 0.4;
        }
    }

    float x_data[512];
    float y_data1[512];
    float y_data2[512];
    float y_data3[512];
};

int main(int argc, char const *argv[])
{
    PlotDemo demo;
    demo.run();
    return 0;
}
