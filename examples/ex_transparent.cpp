#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class TransparentDemo : public Application {
public:
    using Application::Application;

    void draw(NVGcontext* nvg) override {
        auto [w, h] = get_window_size();
        float t = (float)time().as_seconds();
        nvgBeginPath(nvg);
        nvgCircle(nvg, w / 2.0f, h / 2.0f, 100);
        auto paint = nvgRadialGradient(nvg, w / 2.0f, h / 2.0f, 40 + 40 *sin(t), 100, Colors::Yellow, Colors::Transparent);
        nvgFillPaint(nvg, paint);
        nvgFill(nvg);
    }
};

int main(int argc, char const* argv[]) {
    AppConfig conf;
    conf.transparent = true;
    conf.decorated = false;
    conf.resizable = false;
    TransparentDemo demo(conf);
    demo.run();
    return 0;
}
