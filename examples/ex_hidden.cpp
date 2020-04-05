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

using namespace mahi::gui;

class HiddenDemo : public Application {
public:
    HiddenDemo() : Application() { }

    void update() {
        if (open1) {
            ImGui::Begin("Window 1", &open1);
            if (ImGui::Button("Open Window 2"))
                open2 = true;            
            ImGui::End();
        }

        if (open2) {
            ImGui::Begin("Window 2", &open2);
            if (ImGui::Button("Open Window 1"))
                open1 = true;
            ImGui::End();
        }
        if (!open1 && !open2)
            quit();
    }

    bool open1 = true;
    bool open2 = true;
};


int main(int argc, char const *argv[])
{
    HiddenDemo demo;
    demo.run();
    return 0;
}
