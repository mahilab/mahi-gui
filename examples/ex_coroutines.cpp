#include <Mahi/Gui.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class CoroDemo : public Application {
public:
    CoroDemo() : Application(250,250, "Coroutine Demo", false) {  }

    void update() {
        // controls
        ImGui::Begin("Coroutine Demo", nullptr);
        if (ImGui::Button("Move X")) 
            start_coroutine(move(0));
        ImGui::SameLine();
        if (ImGui::Button("Move Y"))
            start_coroutine(move(1));
        ImGui::SameLine();
        if (ImGui::Button("Blink"))
            start_coroutine(blink());
        ImGui::SameLine();
        if (ImGui::Button("Go Crazy"))
            start_coroutine(goCrazy());
        ImGui::Text("Coroutine Count: %d", coroutine_count());
        static float ts = 1;
        if (ImGui::SliderFloat("Time Scale", &ts, 0, 2))
            set_time_scale(ts);
        ImGui::Text("Real Time:  %.3f s", real_time().as_seconds());
        ImGui::Text("Time:       %.3f s", time().as_seconds());
        ImGui::Text("Delta Time: %.3f s", delta_time().as_seconds());
        ImGui::End();
    }

    void draw(NVGcontext* vg) override {
        nvgBeginPath(vg);
        nvgCircle(vg, pos.x, pos.y, 50);
        nvgFillColor(vg, col);
        nvgFill(vg);
    }

    Enumerator move(int axis) {
        Time t;
        while (t < 5_s) {
            pos[axis] = 125 + 50 * (float)std::sin(2*PI*t.as_seconds() + HALFPI * axis);
            t += delta_time();
            co_yield nullptr;
        }
    }

    Enumerator blink() {
        auto col1 = Grays::Black;
        auto col2 = Greens::Chartreuse;
        for (int i = 0; i < 20; ++i) {
            col = col1;
            co_yield yield_time_scaled(100_ms);
            col = col2;
            co_yield yield_time_scaled(150_ms);
        }
    }

    Enumerator goCrazy() {
        start_coroutine(blink());
        start_coroutine(move(0));
        co_yield start_coroutine(move(1));
    }

    Vec2 pos = {125,175};
    Color col = Greens::Chartreuse;
};

int main(int argc, char const *argv[])
{
    CoroDemo demo;
    demo.run();
    return 0;
}
