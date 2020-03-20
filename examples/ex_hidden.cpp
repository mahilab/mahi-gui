#include <Mahi/Gui.hpp>

using namespace mahi::gui;

class HiddenDemo : public Application {
public:
    HiddenDemo() : Application() { }

    void update() {
        if (open1) {
            ImGui::Begin("Window 1", &open1);
            if (ImGui::Button("Open Window 2"))
                open2 = true;            
            ImGui::End();
        }

        if (open2) {
            ImGui::Begin("Window 2", &open2);
            if (ImGui::Button("Open Window 1"))
                open1 = true;
            ImGui::End();
        }

        ImGui::Begin("Test");

        ImGui::End();

        if (!open1 && !open2)
            quit();
    }

    bool open1 = true;
    bool open2 = true;
};


int main(int argc, char const *argv[])
{
    HiddenDemo demo;
    demo.run();
    return 0;
}
