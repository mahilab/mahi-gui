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

class TransparentDemo : public Application {
public:
    using Application::Application;

    void draw(NVGcontext* nvg) override {
        auto [w, h] = get_window_size();
        float t = (float)time().as_seconds();
        nvgBeginPath(nvg);
        nvgCircle(nvg, w / 2.0f, h / 2.0f, 100);
        auto paint = nvgRadialGradient(nvg, w / 2.0f, h / 2.0f, 40 + 40 *sin(t), 100, Colors::Yellow, Colors::Transparent);
        nvgFillPaint(nvg, paint);
        nvgFill(nvg);
    }
};

int main(int argc, char const* argv[]) {
    Application::Config conf;
    conf.transparent = true;
    conf.decorated = false;
    conf.resizable = false;
    TransparentDemo demo(conf);
    demo.run();
    return 0;
}
