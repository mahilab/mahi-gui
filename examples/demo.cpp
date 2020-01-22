#define MAHI_GUI_NO_CONSOLE

#include <mahi/gui.hpp>
#include <iostream>

using namespace mahi::gui;

class MyApp : public Application {
public:
    using Application::Application;
    virtual void update() override {
        ImGui::ShowDemoWindow();
    }
};

int main(int argc, char const *argv[])
{
    MyApp app(600,500,"My App");
    app.run();
    return 0;
}
