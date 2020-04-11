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

// Note: See mahi-gui/misc/svg_ai_export.png for settings to use when exporting from
// Adobe Illustrator.

class SvgDemo : public Application {
public:
    SvgDemo(Config conf) : Application(conf) { }
    ~SvgDemo() {
        if (svg)
            nsvgDelete(svg);
        if (fb)
            nvgluDeleteFramebuffer(fb);
    }

    void update() override {
        auto [win_width, win_height] = get_window_size();
        ImGui::Begin("SVG Demo");
        if (ImGui::Button("Download Public Domain SVG Files"))
            open_url("https://publicdomainvectors.org/");
        if (ImGui::Button("Open File")) {
            if (open_dialog(path, {{"SVG","svg"}}) == DialogOkay) {
                if (svg) {
                    nsvgDelete(svg);
                    svg = nullptr;
                }
                if (fb) {
                    nvgluDeleteFramebuffer(fb);
                    fb = nullptr;
                }
                svg = nsvgParseFromFile(path.c_str(), "px", 96);
                width = svg->width;
                height = svg->height;
                transform = Transformable();
                transform.set_origin(width/2,height/2);
                transform.set_pos(win_width /2, win_height/2);
            }
        }
        ImGui::SameLine();
        ImGui::Text(path.c_str());
        ImGui::BeginDisabled(svg == nullptr);
        if (ImGui::Button("Rasterize"))
            rasterize();
        ImGui::EndDisabled();
        ImGui::Text("Size: %.3f x %.3f px", width, height);
        ImGui::Checkbox("Show Bounds", &draw_bounds);
        ImGui::Separator();
        if (ImGui::Button("VSync On"))
            set_vsync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            set_vsync(false);
        ImGui::SameLine();
        ImGui::Text("FPS: %.3f",  ImGui::GetIO().Framerate);
        if (ImGui::IsMouseDown(0))
            transform.move(ImGui::GetIO().MouseDelta);
        if (ImGui::IsMouseDown(1))
            transform.rotate(ImGui::GetIO().MouseDelta.x * 0.1f);
        if (ImGui::GetIO().MouseWheel > 0)
            transform.scale(1.1,1.1);
        else if (ImGui::GetIO().MouseWheel < 0)
            transform.scale(1 / 1.1, 1 / 1.1);
    }

    void rasterize() {
        if (fb)
            nvgluDeleteFramebuffer(fb);
        fb = nvgluCreateFramebuffer(m_vg, width, height, 0);
        nvgluBindFramebuffer(fb);
        glViewport(0,0,width,height);
        glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(m_vg,width,height,1);
        nvgDrawSvg(m_vg, svg);
        nvgEndFrame(m_vg);
        nvgluBindFramebuffer(nullptr);
        nsvgDelete(svg);
        svg = nullptr;
    }

    void draw(NVGcontext* vg) override {
        nvgTransform(vg, transform.transform());
        if (svg) 
            nvgDrawSvg(vg, svg);        
        else if (fb) {
            NVGpaint paint = nvgImagePattern(vg, 0, 0, width, height, 0, fb->image, 1.0f);
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, width, height);
            nvgFillPaint(vg, paint);
            nvgFill(vg);
        }
        if (draw_bounds) {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, width, height);
            nvgStrokeColor(vg, Whites::White);
            nvgStrokeWidth(vg, 1);
            nvgStroke(vg);
        }
    }

    NSVGimage* svg = nullptr;
    NVGLUframebuffer* fb = nullptr;
    std::string path = "";
    bool draw_bounds = false;
    float width = 0;
    float height = 0;
    Transformable transform;
};

int main(int argc, char const *argv[])
{
    Application::Config conf;
    conf.title = "SVG Demo";
    conf.msaa = 0;
    SvgDemo demo(conf);
    demo.run();
    return 0;
}
