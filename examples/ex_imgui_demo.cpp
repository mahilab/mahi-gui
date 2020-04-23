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
#include <iostream>
#include <tuple>

using namespace mahi::gui;

class Demo : public Application {
public:
    
    Demo() : Application() { }

    virtual void update() override {
        // Official ImGui demo (see imgui_demo.cpp for full example)
        static bool open = true;
        ImGui::ShowDemoWindow(&open);
        if (!open)
            quit();
    }
};

int main(int argc, char const *argv[])
{
    Demo demo;
    demo.run();
    return 0;
}
