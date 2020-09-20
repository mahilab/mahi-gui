#define MAHI_GUI_NO_CONSOLE
#define MAHI_GUI_USE_DISCRETE_GPU
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class ImPlotDemo : public Application {
public:
    ImPlotDemo() : Application() { 
        ImGui::StyleColorsMahiDark3();
    }
    void update() {
        static bool p_open = true;
        ImPlot::ShowDemoWindow(&p_open);
        if (!p_open)
            quit();
    }
};

int main(int argc, char const *argv[])
{
    ImPlotDemo demo;
    demo.run();
    return 0;
}
