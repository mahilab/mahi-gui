#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class FboDemo : public Application {
public:
    FboDemo(Config conf) : Application(conf) { 

    }

    ~FboDemo() {

    }
};


int main(int argc, char const *argv[])
{
    Application::Config conf;
    conf.title = "NanoVG FBO Demo";
    conf.width = 500;
    conf.height = 500;
    conf.msaa = 0;      // pick one
    conf.nvg_aa = true; // pick one
    FboDemo demo(conf);
    demo.run();
    return 0;
}
