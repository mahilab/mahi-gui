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
#include <random>

using namespace mahi::gui;

class DrawingDemo : public Application {
public:
    DrawingDemo() :
        Application("Drawing",0)
    { 
        // disable viewports
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        generate_dots({360,540}, 50, 20000);
        generate_dots({660,540}, 50, 20000);
        generate_dots({960,540}, 50, 20000);
        generate_dots({1260,540}, 50, 20000);
        generate_dots({1560,540}, 50, 20000);
        set_vsync(false);
    }

    virtual void update() override {
        for (auto& group : dotGroups) {
            for (auto& dot : group) {
                ImGui::GetBackgroundDrawList()->AddRectFilled(dot,dot + ImVec2(1,1), dotColor);
            }
        }
    }    

    void generate_dots(ImVec2 center, float radius, int n) {
        std::vector<ImVec2> dots;
        dots.reserve(n);
        for (int i = 0; i < n; ++i) {
            auto loc = center + ImVec2(dist(gen) * radius, dist(gen) * radius);
            dots.push_back(loc);
        }
        dotGroups.push_back(dots);
    }

    std::vector<std::vector<ImVec2>> dotGroups;
    std::default_random_engine gen;
    std::normal_distribution<float> dist;
    ImU32 dotColor = ImGui::GetColorU32({1,1,1,0.25f});
};

int main(int argc, char const *argv[])
{
    DrawingDemo demo;
    demo.run();
    return 0;
}
