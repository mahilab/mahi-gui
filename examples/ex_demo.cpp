// #define MAHI_GUI_NO_CONSOLE

#include <Mahi/Gui.hpp>
#include <iostream>
#include <tuple>

using namespace mahi::gui;

class Demo : public Application {
public:
    
    Demo() : Application() { }

    virtual void update() override {
        // Official ImGui demo (see imgui_demo.cpp for full example)
        ImGui::ShowDemoWindow();
        // A custom Window
        ImGui::Begin("My ImGui Window");
        if (ImGui::Button(ICON_FA_HOME))
            open_url("https://mahilab.rice.edu/");
        ImGui::SameLine();
        if (ImGui::Button("Quit Demo", ImVec2(-1,0)))
            quit();
        ImGui::End();
    }
};

int main(int argc, char const *argv[])
{
    Demo demo;
    demo.run();
    return 0;
}
