// #define MAHI_GUI_NO_CONSOLE

#include <mahi/gui.hpp>
#include <iostream>

using namespace mahi::gui;

class MyApp : public Application {
public:
    using Application::Application;
    virtual void update() override {
        ImGui::ShowDemoWindow();
        ImGui::Begin("My ImGui Window");
        if (ImGui::Button("Hello, World"))
            std::cout << "Hello, World" << std::endl;
        if (ImGui::Button(ICON_FA_GITHUB))          
            std::cout << "Download Button" << std::endl;
        ImGui::End();
    }
};

int main(int argc, char const *argv[])
{
    for (int i : range(0,10))
        print(i);

    MyApp app(600,500,"My App");
    app.run();
    return 0;
}
