#include <MAHUI/Application.hpp>
#include <iostream>

class MyApp : public Application {
public:
    using Application::Application;
    virtual void update() override {
        ImGui::Begin("My ImGui Window");
        if (ImGui::Button("Hello, World"))
            std::cout << "Hellow, World" << std::endl;
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
};

int main(int argc, char const *argv[])
{
    MyApp app(600,500,"My App");
    app.run();
    return 0;
}
