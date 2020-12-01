#include <Mahi/Gui.hpp>

using namespace mahi::gui;
using namespace mahi::util;

struct SquarePiece : public Transformable {
    void draw(NVGcontext* vg) {
        nvgTransform(vg, transform());
        nvgBeginPath(vg);
        nvgRect(vg, -size * 0.5f, -size * 0.5f, size, size);
        nvgFillColor(vg, color);
        nvgFill(vg);
    }

    Color color = Greens::Chartreuse;
    float size  = 20;
};

class SeqDemo : public Application {
public:
    SeqDemo() : Application(250, 250, "Sequence Demo", false) { square.set_pos(20, 20); }

    void update() {
        static float t = 3;
        ImGui::Begin("Sequence Demo", nullptr);
        ImGui::DragFloat("Time", &t, 0.1f, 1, 5);
        if (ImGui::Button("Move"))
            start_coroutine(move(t));
        ImGui::SameLine();
        if (ImGui::Button("Rotate"))
            start_coroutine(rotate(t));
        ImGui::SameLine();
        if (ImGui::Button("Blend"))
            start_coroutine(blend(t));
        ImGui::SameLine();
        ImGui::End();
    }

    void draw(NVGcontext* vg) override { square.draw(vg); }

    Enumerator move(float duration) {
        Sequence<Vec2> path(Tween::Smootherstep);
        path[0.00f] = Vec2(20, 20);
        path[0.25f] = Vec2(20, 230);
        path[0.50f] = Vec2(230, 230);
        path[0.75f] = Vec2(230, 20);
        path[1.00f] = Vec2(20, 20);
        Time t;
        while (t < seconds(duration)) {
            float x = (float)t.as_seconds() / duration;
            auto  p = path(x);
            square.set_pos(p);
            t += delta_time();
            co_yield nullptr;
        }
        square.set_pos(20, 20);
    }

    Enumerator rotate(float duration) {
        Sequence<float> angles(Tween::Exponential::In);
        angles[0.00f] = 0;
        angles[0.25f] = 90;
        angles[0.50f] = 180;
        angles[0.75f] = 270;
        angles[1.00f] = 360;
        Time t;
        while (t < seconds(duration)) {
            float x = (float)t.as_seconds() / duration;
            auto  r = angles(x);
            square.set_rotation(r);
            t += delta_time();
            co_yield nullptr;
        }
        square.set_rotation(0);
    }

    Enumerator blend(float duration) {
        Sequence<Color> gradient(Tween::Linear);
        gradient[0.00f] = Greens::Chartreuse;
        gradient[0.25f] = Blues::DeepSkyBlue;
        gradient[0.50f] = Reds::FireBrick;
        gradient[0.75f] = Yellows::Yellow;
        gradient[1.00f] = Greens::Chartreuse;
        Time t;
        while (t < seconds(duration)) {
            float x      = (float)t.as_seconds() / duration;
            auto  c      = gradient(x);
            square.color = c;
            t += delta_time();
            co_yield nullptr;
        }
        square.color = Greens::Chartreuse;
    }

    SquarePiece square;
};

int main(int argc, char const* argv[]) {
    SeqDemo demo;
    demo.run();
    return 0;
}
