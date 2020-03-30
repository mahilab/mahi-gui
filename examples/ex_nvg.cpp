#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include "demo.h"

using namespace mahi::gui;
using namespace mahi::util;

class NvgDemo : public Application {
public:
    NvgDemo(Config conf) : Application(conf) { 
        set_background({0.3f, 0.3f, 0.32f, 1.0f});
        loadDemoData(m_nvg,&data);
    }

    ~NvgDemo() {
        freeDemoData(m_nvg,&data);
    }

    void update() override { 
        ImGui::Begin("ImGui");
        ImGui::Text("ImGui will always be drawn on top of NanoVG");
        ImGui::Text("even if it is called first in update()");
        if (ImGui::Button("VSync On"))
            set_vsync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            set_vsync(false);
        ImGui::SameLine();
        ImGui::Text("%.3f FPS", ImGui::GetIO().Framerate);
        static int lim = 120;
        ImGui::SliderInt("##Limit", &lim, 0, 500, "%d Hz"); ImGui::SameLine();
        if (ImGui::Button("Limit"))
            set_frame_limit(hertz(lim));
        ImGui::End();
    }

    void draw(NVGcontext* vg) override {
        auto [x,y] = get_mouse_pos();
        auto [w,h] = get_window_size();
        float t = (float)time().as_seconds();
        renderDemo(vg,x,y,w,h,t,0,&data);
        
    }    

    DemoData data;
};



int main(int argc, char const *argv[])
{
    Application::Config conf;
    conf.title = "NanoVG Demo";
    conf.width = 1000;
    conf.height = 600;
    conf.msaa = 0;      // pick one
    conf.nvg_aa = true; // pick one
    NvgDemo demo(conf);
    demo.run();
    return 0;
}



