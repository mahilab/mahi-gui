#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <thread>

using namespace mahi::gui;
using namespace mahi::util;

class FileDemo : public Application {
public:
    FileDemo() : Application(150,150,"File Demo", false) {
        // disable viewports
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        // connect to the file drop event
        onFileDrop.connect(this, &FileDemo::fileDropHandler);        
    }
    
    void update() override {
        ImGui::SetNextWindowPos({0,0}, ImGuiCond_Always);
        ImGui::SetNextWindowSize({150,150}, ImGuiCond_Always);
        ImGui::Begin("Save Dialog", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        if (ImGui::Button(ICON_FA_SAVE)) {
            if (saveDialog("png,jpeg;pdf", "", out) == DialogResult::DialogOkay)
                print("Path: {}",out);
        }           
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FILE)) {
            if (openDialog("png,jpeg;pdf", "", out) == DialogResult::DialogOkay)
                print("Path: {}",out);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_COPY)) {
            if (openDialog("png,jpeg;pdf", "", outs) == DialogResult::DialogOkay)
            {
                for (auto& o : outs)
                    print("Path: {}", o);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN)) {
            if (pickFolder("", out) == DialogResult::DialogOkay)
                print("Path: {}",out); 
        }
        ImGui::End();
    }

    void fileDropHandler(const std::vector<std::string>& paths) {
        for (auto& p : paths) 
            std::cout << p  << std::endl;
    }
    
    std::string out;
    std::vector<std::string> outs;    
};

int main( void )
{
    FileDemo demo;
    demo.run();
    return 0;
}
