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
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

using namespace mahi::gui;
using namespace mahi::util;

struct Dot {
    Dot(Application& application) : app(application) {
        float r = random_range(0.0, 250.0);
        float a = random_range(0.0, 2 * PI);
        pos0.x = 300 + r * std::cos(a);
        pos0.y = 300 + r * std::sin(a);
        seed   = random_range(0, 100000);
        speed  = (float)random_range(1.0f, 3.0f);
        color  = random_color();
    }

    void draw(NVGcontext* vg) {
        nvgBeginPath(vg);
        nvgRect(vg, pos.x - 1, pos.y - 1, 2, 2);
        nvgFillColor(vg, color);
        nvgFill(vg);
    }

    void animate() {
        t += (float)app.delta_time().as_seconds() * speed;
        pos.x = pos0.x + 100 * stb_perlin_noise3_seed(t, 0, 0, 0, 0, 0, seed);
        pos.y = pos0.y + 100 * stb_perlin_noise3_seed(0, t, 0, 0, 0, 0, seed);
    }

    Enumerator animate_coro() {
        while (true) {
            animate();
            co_yield nullptr;
        }
    }

    Application& app;
    Vec2         pos0,      pos;
    int          seed;
    float        speed;
    float        t = 0;
    Color        color;
};

class CoroDemo : public Application {
public:
    CoroDemo(Config conf) : Application(conf) { 
        dots.reserve(5000);
        for (int i = 0; i < 5000; ++i)
            dots.emplace_back(*this);
        set_background(Grays::Gray90);
        set_vsync(false);
    }

    void update() {
        ImGui::Begin("Coroutines Benchmark", &open);
        if (ImGui::Checkbox("Use Coroutines", &use_coros)) {
            if (!use_coros)
                stop_coroutines();
            else {
                for (auto& dot : dots)
                    start_coroutine(dot.animate_coro());
            }
        }
        ImGui::Text("Coroutines: %d", coroutine_count());
        ImGui::Text("%.3f FPS", ImGui::GetIO().Framerate);
        ImGui::End();
        if (!use_coros) {
            for (auto& dot : dots) 
                dot.animate();            
        }

        if (!open)
            quit();
    }

    void draw(NVGcontext* vg) override {
        for (auto& dot : dots) 
            dot.draw(vg);        
    }

    std::vector<Dot> dots;
    bool use_coros = false;
    bool open = true;
};

int main(int argc, char const* argv[]) {
    Application::Config conf;
    conf.height = 600;
    conf.width = 600;
    conf.title = "Coroutine Benchmark";
    conf.resizable = false;
    conf.transparent = true;
    conf.decorated = false;
    CoroDemo demo(conf);
    demo.run();
    return 0;
}