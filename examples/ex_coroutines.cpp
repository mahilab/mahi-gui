#include <Mahi/Gui.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class CoroDemo : public Application {
public:
    CoroDemo() : Application(250,250, "Coroutine Demo", false) { 
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }

    void update() {
        // controls
        ImGui::SetNextWindowPos({10,10});
        ImGui::Begin("Coro Demo", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
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
        ImGui::End();
        // draw
        ImGui::GetBackgroundDrawList()->AddCircleFilled(pos, 50, col, 100);
    }

    Enumerator move(int axis) {
        Time t;
        while (t < 5_s) {
            pos[axis] = 125 + 50 * (float)std::sin(2*PI*t.as_seconds() + HALFPI * axis);
            t += dt();
            co_yield nullptr;
        }
    }

    Enumerator blink() {
        auto col1 = ImGui::GetColorU32(Grays::Black);
        auto col2 = ImGui::GetColorU32(Greens::Chartreuse);
        for (int i = 0; i < 20; ++i) {
            col = col1;
            co_yield new WaitForSeconds(0.1f);
            col = col2;
            co_yield new WaitForSeconds(0.15f);
        }
    }

    Enumerator goCrazy() {
        start_coroutine(blink());
        start_coroutine(move(0));
        co_yield start_coroutine(move(1));
    }

    Vec2 pos = {125,175};
    ImU32 col = ImGui::GetColorU32(Greens::Chartreuse);
};

int main(int argc, char const *argv[])
{
    CoroDemo demo;
    demo.run();
    return 0;
}
