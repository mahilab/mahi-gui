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
#include <random>

using namespace mahi::gui;
using namespace mahi::util;

// THIS EXAMPLE CURRENTLY DOES NOT WORK, DPI IS A WIP!!!

class DpiDemo : public Application {
public:
    DpiDemo(Config conf) : Application(conf) {
        ImGui::DisableViewports();
    }
    virtual void update() override { 
        float dpi = get_dpi_scale();
        ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(640,480), ImGuiCond_Appearing);
        ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("DPI Scale: %.2f", dpi);
        ImGui::Button("Press Me");
        ImGui::End();
    }   
};

int main(int argc, char *argv[])
{
    // THIS EXAMPLE CURRENTLY DOES NOT WORK, DPI IS A WIP!!!
    Options options("dpi.exe", "DPI Demo");
    options.add_options()("d,dpi","Enables DPI Awareness");
    auto result = options.parse(argc, argv);

    Application::Config cfg;
    cfg.dpi_aware = result.count("d") > 0;
    cfg.width  = 640;
    cfg.height = 480;
    cfg.title  = "DPI Demo";

    DpiDemo demo(cfg);
    demo.run();
    return 0;
}
