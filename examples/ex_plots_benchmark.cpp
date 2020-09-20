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

// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

struct PlotItem {
    std::vector<ImVec2> data;
    ImVec4 color;
    std::string label;
};

class PlotBench : public Application {
public:
    PlotBench() : Application(500,500,"Plots Benchmark") { 
        set_vsync(false);
        ImGui::DisableViewports();
        for (int i = 0; i < 100; ++i)
        {
            PlotItem item;
            item.data.reserve(1000);
            item.color = random_color();
            item.label = fmt::format("item_{}",i);
            float y = i * 0.01f;
            for (int i = 0; i < 1000; ++i)
                item.data.push_back(ImVec2(i*0.001f, y + (float)random_range(-0.01,0.01)));
            items.emplace_back(std::move(item));
        }
    };
    void update() override {
        if (animate) {
            for (int i = 0; i < 100; ++i) {
                float y = i * 0.01f;
                for (int j = 0; j < 1000; ++j) {
                    items[i].data[j].x = j*0.001f;
                    items[i].data[j].y = y + (float)random_range(-0.01,0.01);
                }
            }
        }
        auto [w,h] = get_window_size();
        ImGui::SetNextWindowPos({0,0}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(w,h), ImGuiCond_FirstUseEver);
        ImGui::Begin("Plot Benchmark",nullptr, ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button("VSync On"))
            set_vsync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            set_vsync(false);
        ImGui::SameLine();
        ImGui::Checkbox("Render", &render);
        ImGui::SameLine();
        ImGui::Checkbox("Animate", &animate);
        ImGui::Text("%lu lines, 1000 pts ea. @ %.3f FPS", items.size(), ImGui::GetIO().Framerate);
        if (ImPlot::BeginPlot("##Plot", NULL, NULL, ImVec2(-1,-1), ImPlotFlags_NoChild)) {
            if (render) {         
                for (int i = 0; i < 100; ++i) {
                    ImPlot::PushStyleColor(ImPlotCol_Line, items[i].color);
                    ImPlot::PlotLine(items[i].label.c_str(), &items[i].data[0].x, &items[i].data[0].y, items[i].data.size(), 0, 8);
                    ImPlot::PopStyleColor();
                }
            }
            ImPlot::EndPlot();
        }        
        ImGui::End();
        // ImGui::ShowMetricsWindow();
    }

    std::vector<PlotItem> items;
    float v[2] = {1,2};
    bool render = true;
    bool animate = false;
};

int main(int argc, char const *argv[])
{
    PlotBench app;
    app.run();
    return 0;
}
