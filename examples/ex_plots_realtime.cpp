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

using namespace mahi::gui;
using namespace mahi::util;

class RealTimePlotDemo : public Application {
public:
    RealTimePlotDemo() : Application(800,500,"Real Time Plot Demo") { 
        ImGui::DisableViewports();
        items1.resize(1);
        plot1.x_axis.maximum = 10;
        plot1.x_axis.lock_min = true;  plot1.x_axis.lock_max = true;
        plot1.title = "ImGui::PlotItemRollPoint(...)";
        items2.resize(3);
        plot2.x_axis.maximum = 10;
        plot2.x_axis.lock_min = true;  plot2.x_axis.lock_max = true;
        plot2.title = "ImGui::PlotItemBufferPoint(...)";
    }

    void update() override {
        auto [w,h] = get_window_size();
        ImGui::SetNextWindowPos({0,0}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({w,h}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Real Time Plots", nullptr, ImGuiWindowFlags_NoTitleBar);  
        float regionHeight = ImGui::GetContentRegionAvail().y;   
        float t = time().as_seconds();
        auto [x,y] = get_mouse_pos();
        // rolling plot example
        ImGui::PlotItemRollPoint(items1[0], t, + x*0.0005f + random_range(0.49f,0.51f), 10.0f);
        ImGui::Plot("My Rolling Plot", plot1, items1, ImVec2(-1,regionHeight/2));
        // buffering plot example
        ImGui::PlotItemBufferPoint(items2[0], t, x*0.0005f + random_range(0.49f,0.51f), 1000);
        ImGui::PlotItemBufferPoint(items2[1], t, y*0.0005f + random_range(0.49f,0.51f), 1000);
        ImGui::PlotItemBufferPoint(items2[2], t, (x+y)*0.00025f + random_range(0.49f,0.51f), 1000);
        ImGui::PlotAxisScroll(plot2.x_axis, t, 10);
        ImGui::Plot("My Buffering Plot", plot2, items2, ImVec2(-1,-1));
        ImGui::End();
    }

    ImGui::PlotInterface plot1, plot2;
    std::vector<ImGui::PlotItem> items1, items2;
};

int main(int argc, char const *argv[])
{
    RealTimePlotDemo demo;
    demo.run();
    return 0;
}