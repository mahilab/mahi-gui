// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <thread>
#include <mutex>

using namespace mahi::gui;
using namespace mahi::util;

class FileDemo : public Application {
public:
    FileDemo() : Application(300,400,"File Demo", false) {
        // disable viewports
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        // connect to the file drop event
        on_file_drop.connect(this, &FileDemo::fileDropHandler);        
    }
    
    void update() override {
        ImGui::SetNextWindowPos({0,0}, ImGuiCond_Always);
        ImGui::SetNextWindowSize({300,400}, ImGuiCond_Always);
        ImGui::Begin("Save Dialog", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::Text("Non-threaded:");
        if (ImGui::Button(ICON_FA_SAVE"##1")) {
            if (save_dialog(out, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}, "", "my_file") == DialogResult::DialogOkay)
                print("Path: {}",out);
        }           
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FILE"##1")) {
            if (open_dialog(out, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}) == DialogResult::DialogOkay)
                print("Path: {}",out);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_COPY"##1")) {
            if (open_dialog(outs, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}) == DialogResult::DialogOkay) {
                for (auto& o : outs)
                    print("Path: {}", o);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN"##1")) {
            if (pick_dialog(out) == DialogResult::DialogOkay)
                print("Path: {}",out); 
        }

        ImGui::Text("Threaded:");
        if (ImGui::Button(ICON_FA_SAVE"##2")) {
            auto func = [this]() {
                std::lock_guard<std::mutex> lock(mtx);
                if (save_dialog(out, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}, "", "my_file") == DialogResult::DialogOkay)
                    print("Path: {}",out);
            };
            std::thread thrd(func); thrd.detach();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FILE"##2")) {
            auto func = [this]() {
                std::lock_guard<std::mutex> lock(mtx);
                if (open_dialog(out, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}) == DialogResult::DialogOkay)
                    print("Path: {}",out);
            };
            std::thread thrd(func); thrd.detach();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_COPY"##2")) {
            auto func = [this]() {
                std::lock_guard<std::mutex> lock(mtx);
                if (open_dialog(outs, {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}}) == DialogResult::DialogOkay) {
                    for (auto& o : outs)
                        print("Path: {}", o);
                }
            };
            std::thread thrd(func); thrd.detach();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN"##2")) {
            auto func = [this]() {
                std::lock_guard<std::mutex> lock(mtx);
                if (pick_dialog(out) == DialogResult::DialogOkay)
                    print("Path: {}",out); 
            };
            std::thread thrd(func); thrd.detach();
        }

        // You should notice that the non-threaded buttons freeze the GUI, while
        // threaded buttons do not. Be careful with threads and always mutex data.
        ImGui::Text("%.3f",time().as_seconds());
        ImGui::Separator();
        ImGui::Text("System Folders:");

        static auto sys_dir_but = [](SysDir dir) {
            if (ImGui::Button(sys_dir(dir).c_str()))
                open_folder(sys_dir(dir));
        };

        sys_dir_but(UserProfile);
        sys_dir_but(AppDataRoaming);
        sys_dir_but(AppDataLocal);
        sys_dir_but(AppDataTemp);
        sys_dir_but(ProgramData);
        sys_dir_but(ProgramFiles);
        sys_dir_but(ProgramFilesX86);

        ImGui::Separator();

        ImGui::Text("Misc:");
        if (ImGui::Button("github.com/mahilab"))
            open_url("https://github.com/mahilab");
        if (ImGui::Button("epezent@rice.edu"))
            open_email("epezent@rice.edu", "I love mahi-gui!");

        ImGui::End();
    }

    void fileDropHandler(const std::vector<std::string>& paths) {
        for (auto& p : paths) 
            std::cout << p  << std::endl;
    }
    
    std::mutex mtx;
    std::string out;
    std::vector<std::string> outs;    
};

int main( void )
{
    FileDemo demo;
    demo.run();
    return 0;
}
