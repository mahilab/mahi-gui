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
#include <cstring>

using namespace mahi::gui;
using namespace mahi::util;

// Note: See mahi-gui/misc/svg_ai_export.png for settings to use when exporting from
// Adobe Illustrator.

extern std::string mahi_logo;

class SvgDemo : public Application {
public:
    SvgDemo(Config conf) : Application(conf) { 
        svg = nsvgParseFromString(mahi_logo, "px", 96);
        width = svg->width;
        height = svg->height;
        transform = Transformable();
        transform.set_origin(width/2,height/2);
        transform.set_pos(get_window_size() * 0.5f);
    }
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
        ImGui::TextUnformatted(path.c_str());
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
        ImGui::End();
        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow)) {
            if (ImGui::IsMouseDown(0))
                transform.move(ImGui::GetIO().MouseDelta);
            if (ImGui::IsMouseDown(1))
                transform.rotate(ImGui::GetIO().MouseDelta.x * 0.1f);
            if (ImGui::GetIO().MouseWheel > 0)
                transform.scale(1.1,1.1);
            else if (ImGui::GetIO().MouseWheel < 0)
                transform.scale(1 / 1.1, 1 / 1.1);
        }
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

std::string mahi_logo = R"(<?xml version="1.0" encoding="utf-8"?>
<!-- Generator: Adobe Illustrator 24.1.1, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
     width="600px" height="210px" viewBox="0 0 600 210" style="enable-background:new 0 0 600 210;" xml:space="preserve">
<path style="fill:#FFFFFF;" d="M150.89,49.5c36.21,0,54.31,18.51,54.31,55.52v83.28c0,12.07-6.03,18.1-18.1,18.1
    c-12.07,0-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.16-18.59-18.47-18.59h-30.05l0.24,102.59c0,12.07-6.03,18.1-18.1,18.1
    s-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.12-18.59-18.35-18.59H36.23v102.1c0,12.23-6.12,18.35-18.35,18.35
    c-11.91,0-17.86-6.11-17.86-18.35V67.48c0-11.91,6.16-17.98,18.47-18.22L150.89,49.5z"/>
<path style="fill:#FFFFFF;" d="M344.11,206.4c-12.31,0-18.47-6.11-18.47-18.35V158l-59.98-0.12l-0.12,30.17
    c0,12.23-6.03,18.35-18.1,18.35c-12.07,0-18.1-6.03-18.1-18.1v-83.28c0-37.01,18.1-55.52,54.31-55.52l60.1,0.12
    c12.07,0,18.1,5.96,18.1,17.86v120.57C361.85,200.28,355.94,206.4,344.11,206.4z M325.65,121.55V85.7h-41.64
    c-12.31,0-18.47,6.2-18.47,18.59v17.26H325.65z"/>
<path style="fill:#FFFFFF;" d="M386.48,188.05V18.84c0-11.99,6.03-17.98,18.1-17.98s18.1,6.16,18.1,18.47V49.5h42.48
    c36.21,0,54.31,18.51,54.31,55.52v83.28c0,12.07-6.03,18.1-18.1,18.1s-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.12-18.59-18.35-18.59
    h-42.24v102.35c0,12.23-6.12,18.35-18.35,18.35C392.43,206.4,386.48,200.28,386.48,188.05z"/>
<path style="fill:#FFFFFF;" d="M580.31,206.15c-12.23,0-18.35-6.11-18.35-18.35V85.7c-12.23,0-18.27-5.83-18.1-17.5
    c0-12.23,6.03-18.47,18.1-18.71l18.1-0.24c12.07,0.24,18.1,6.32,18.1,18.22v120.33C598.17,200.04,592.21,206.15,580.31,206.15z
     M570.23,3.39c-2.86,1.53-5.09,3.74-6.7,6.64c-1.61,2.9-2.41,5.91-2.41,9.05c0,3.06,0.8,6.02,2.41,8.87
    c1.61,2.86,3.82,5.07,6.64,6.64c2.82,1.57,5.79,2.35,8.93,2.35c3.06,0,6.01-0.78,8.87-2.35c2.85-1.57,5.07-3.78,6.64-6.64
    c1.57-2.85,2.35-5.81,2.35-8.87c0-3.14-0.8-6.16-2.41-9.05c-1.61-2.9-3.84-5.11-6.7-6.64c-2.86-1.53-5.77-2.29-8.75-2.29
    C576.04,1.1,573.08,1.87,570.23,3.39z"/>
<path style="fill:#5D5BBA;" d="M152.74,51.35c36.21,0,54.31,18.51,54.31,55.52v83.28c0,12.07-6.03,18.1-18.1,18.1
    s-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.16-18.59-18.47-18.59h-30.05l0.24,102.59c0,12.07-6.03,18.1-18.1,18.1
    s-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.12-18.59-18.35-18.59H38.08v102.1c0,12.23-6.12,18.35-18.35,18.35
    c-11.91,0-17.86-6.11-17.86-18.35V69.33c0-11.91,6.16-17.98,18.47-18.22L152.74,51.35z"/>
<path style="fill:#5D5BBA;" d="M345.97,208.25c-12.31,0-18.47-6.11-18.47-18.35v-30.05l-59.98-0.12l-0.12,30.17
    c0,12.23-6.03,18.35-18.1,18.35c-12.07,0-18.1-6.03-18.1-18.1v-83.28c0-37.01,18.1-55.52,54.31-55.52l60.1,0.12
    c12.07,0,18.1,5.96,18.1,17.86V189.9C363.71,202.14,357.79,208.25,345.97,208.25z M327.5,123.4V87.56h-41.64
    c-12.31,0-18.47,6.2-18.47,18.59v17.26H327.5z"/>
<path style="fill:#5D5BBA;" d="M388.33,189.9V20.7c0-11.99,6.03-17.98,18.1-17.98c12.07,0,18.1,6.16,18.1,18.47v30.17h42.48
    c36.21,0,54.31,18.51,54.31,55.52v83.28c0,12.07-6.03,18.1-18.1,18.1s-18.1-6.11-18.1-18.35v-83.76c0-12.39-6.12-18.59-18.35-18.59
    h-42.24V189.9c0,12.23-6.12,18.35-18.35,18.35C394.28,208.25,388.33,202.14,388.33,189.9z"/>
<g>
    <path style="fill:#5D5BBA;" d="M582.16,208.01c-12.23,0-18.35-6.11-18.35-18.35V87.56c-12.23,0-18.27-5.83-18.1-17.5
        c0-12.23,6.03-18.47,18.1-18.71l18.1-0.24c12.07,0.24,18.1,6.32,18.1,18.22v120.33C600.02,201.89,594.07,208.01,582.16,208.01z
         M572.08,5.25c-2.86,1.53-5.09,3.74-6.7,6.64c-1.61,2.9-2.41,5.91-2.41,9.05c0,3.06,0.8,6.02,2.41,8.87
        c1.61,2.86,3.82,5.07,6.64,6.64c2.82,1.57,5.79,2.35,8.93,2.35c3.06,0,6.01-0.78,8.87-2.35c2.85-1.57,5.07-3.78,6.64-6.64
        c1.57-2.85,2.35-5.81,2.35-8.87c0-3.14-0.8-6.16-2.41-9.05c-1.61-2.9-3.84-5.11-6.7-6.64c-2.86-1.53-5.77-2.29-8.75-2.29
        C577.89,2.95,574.94,3.72,572.08,5.25z"/>
</g>
<g style="opacity:0.26;">
    <linearGradient id="SVGID_1_" gradientUnits="userSpaceOnUse" x1="104.4659" y1="51.1122" x2="104.4659" y2="137.8026">
        <stop  offset="0" style="stop-color:#A2A0D7"/>
        <stop  offset="1" style="stop-color:#5D5BBA"/>
    </linearGradient>
    <path style="fill:url(#SVGID_1_);" d="M207.05,106.87v30.93c-12.25-0.61-24.33-1.33-36.2-2.18v-29.48
        c0-12.39-6.16-18.59-18.47-18.59h-30.05l0.11,43.89c-12.31-1.25-24.34-2.64-36.07-4.15v-21.16c0-12.39-6.12-18.59-18.35-18.59
        H38.08v32.58c-12.55-2.13-24.63-4.41-36.2-6.83V69.34c0-11.91,6.15-17.98,18.46-18.22l132.4,0.24
        C188.95,51.35,207.05,69.86,207.05,106.87z"/>
</g>
<g>
    <linearGradient id="SVGID_2_" gradientUnits="userSpaceOnUse" x1="104.4659" y1="137.8026" x2="104.4659" y2="51.1122">
        <stop  offset="0" style="stop-color:#CECDEA;stop-opacity:0.1"/>
        <stop  offset="1" style="stop-color:#FFFFFF;stop-opacity:0.5"/>
    </linearGradient>
    <path style="fill:url(#SVGID_2_);" d="M207.05,106.87v30.93c-12.25-0.61-24.33-1.33-36.2-2.18v-29.48
        c0-12.39-6.16-18.59-18.47-18.59h-30.05l0.11,43.89c-12.31-1.25-24.34-2.64-36.07-4.15v-21.16c0-12.39-6.12-18.59-18.35-18.59
        H38.08v32.58c-12.55-2.13-24.63-4.41-36.2-6.83V69.34c0-11.91,6.15-17.98,18.46-18.22l132.4,0.24
        C188.95,51.35,207.05,69.86,207.05,106.87z"/>
</g>
<g>
    <g>
        <linearGradient id="SVGID_3_" gradientUnits="userSpaceOnUse" x1="297.4478" y1="140.0519" x2="297.4478" y2="51.3532">
            <stop  offset="0" style="stop-color:#CECDEA;stop-opacity:0.1"/>
            <stop  offset="1" style="stop-color:#FFFFFF;stop-opacity:0.5"/>
        </linearGradient>
        <path style="fill:url(#SVGID_3_);" d="M345.61,51.47l-60.11-0.12c-36.2,0-54.31,18.51-54.31,55.52v31.97
            c22.05,0.8,44.62,1.21,67.58,1.21c22.05,0,43.73-0.38,64.93-1.12v-69.6C363.71,57.43,357.67,51.47,345.61,51.47z M327.5,123.4
            h-60.1v-17.26c0-12.39,6.15-18.59,18.46-18.59h41.64V123.4z"/>
    </g>
</g>
<g>
    <linearGradient id="SVGID_4_" gradientUnits="userSpaceOnUse" x1="454.8302" y1="137.9077" x2="454.8302" y2="2.7154">
        <stop  offset="0" style="stop-color:#CECDEA;stop-opacity:0.1"/>
        <stop  offset="1" style="stop-color:#FFFFFF;stop-opacity:0.5"/>
    </linearGradient>
    <path style="fill:url(#SVGID_4_);" d="M521.33,106.87v19.09c-11.75,1.61-23.83,3.08-36.2,4.43v-24.25
        c0-12.39-6.12-18.59-18.35-18.59h-42.24v48.22c-11.88,0.82-23.95,1.54-36.2,2.13V20.7c0-11.99,6.03-17.98,18.1-17.98
        c12.07,0,18.11,6.15,18.11,18.46v30.17h42.48C503.23,51.35,521.33,69.86,521.33,106.87z"/>
</g>
<g>
    <linearGradient id="SVGID_5_" gradientUnits="userSpaceOnUse" x1="572.8669" y1="119.3943" x2="572.8669" y2="51.1122">
        <stop  offset="0" style="stop-color:#CECDEA;stop-opacity:0.1"/>
        <stop  offset="1" style="stop-color:#FFFFFF;stop-opacity:0.5"/>
    </linearGradient>
    <path style="fill:url(#SVGID_5_);" d="M600.02,69.34v43.05c-11.56,2.48-23.65,4.82-36.21,7.01V87.56c-12.23,0-18.27-5.83-18.1-17.5
        c0-12.23,6.03-18.46,18.1-18.7l18.11-0.24C593.99,51.35,600.02,57.43,600.02,69.34z"/>
</g>
<g>
    <path style="opacity:0.5;fill:#FFFFFF;" d="M598.81,20.94c0,3.06-0.78,6.01-2.35,8.87c-1.56,2.86-3.78,5.07-6.64,6.64
        c-2.85,1.57-5.81,2.35-8.87,2.35c-3.14,0-6.12-0.78-8.94-2.35c-2.82-1.56-5.03-3.78-6.64-6.64c-1.61-2.85-2.42-5.81-2.42-8.87
        c0-3.14,0.8-6.15,2.42-9.05c1.61-2.9,3.84-5.11,6.7-6.64c2.85-1.53,5.81-2.29,8.87-2.29c2.97,0,5.89,0.77,8.75,2.29
        c2.85,1.53,5.09,3.74,6.7,6.64C598.01,14.78,598.81,17.8,598.81,20.94z"/>
</g>
</svg>)";
