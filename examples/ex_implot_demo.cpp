#define MAHI_GUI_NO_CONSOLE
#define MAHI_GUI_USE_DISCRETE_GPU
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotDemo : public Application {
public:
    PlotDemo() : Application() { }
    void update() {
        static bool p_open = true;
        ImPlot::ShowDemoWindow(&p_open);
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
