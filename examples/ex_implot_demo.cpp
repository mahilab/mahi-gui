// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class PlotApi : public Application {
public:
    PlotApi() : Application() { }

    void update() {
        static bool p_open = true;
        ImGui::ShowImPlotDemoWindow(&p_open);
        if (!p_open)
            quit();
    }
};

int main(int argc, char const *argv[])
{

    PlotApi api;
    api.run();
    return 0;
}
