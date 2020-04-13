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

class FboDemo : public Application {
public:
    FboDemo(Config conf) : Application(conf) { 
        auto pxRatio = get_pixel_ratio();
	    m_fb = nvgluCreateFramebuffer(m_vg, (int)(100*pxRatio), (int)(100*pxRatio), NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
        set_background({0.3f, 0.3f, 0.32f, 1.0f});
    }

    ~FboDemo() {
	    nvgluDeleteFramebuffer(m_fb);
    }

    void update() override {
        renderPattern(m_vg, m_fb, time().as_seconds(), get_pixel_ratio());
    }

    void draw(NVGcontext* vg) override {
        float t = time().as_seconds();
		if (m_fb != NULL) {
			NVGpaint img = nvgImagePattern(vg, 0, 0, 100, 100, 0, m_fb->image, 1.0f);
			nvgSave(vg);
			for (int i = 0; i < 20; i++) {
				nvgBeginPath(vg);
				nvgRect(vg, 10 + i*30,10, 10, get_window_size().y - 20);
				nvgFillColor(vg, nvgHSLA(i/19.0f, 0.5f, 0.5f, 255));
				nvgFill(vg);
			}
			nvgBeginPath(vg);
			nvgRoundedRect(vg, 140 + sinf(t*1.3f)*100, 140 + cosf(t*1.71244f)*100, 250, 250, 20);
			nvgFillPaint(vg, img);
			nvgFill(vg);
			nvgStrokeColor(vg, nvgRGBA(220,160,0,255));
			nvgStrokeWidth(vg, 3.0f);
			nvgStroke(vg);
			nvgRestore(vg);
		}
    }

    void renderPattern(NVGcontext* vg, NVGLUframebuffer* fb, float t, float pxRatio)
    {
        int winWidth, winHeight;
        int fboWidth, fboHeight;
        int pw, ph, x, y;
        float s = 20.0f;
        float sr = (cosf(t)+1)*0.5f;
        float r = s * 0.6f * (0.2f + 0.8f * sr);

        if (fb == NULL) return;

        nvgImageSize(vg, fb->image, &fboWidth, &fboHeight);
        winWidth = (int)(fboWidth / pxRatio);
        winHeight = (int)(fboHeight / pxRatio);

        // Draw some stuff to an FBO as a test
        nvgluBindFramebuffer(fb);
        glViewport(0, 0, fboWidth, fboHeight);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

        pw = (int)ceilf(winWidth / s);
        ph = (int)ceilf(winHeight / s);

        nvgBeginPath(vg);
        for (y = 0; y < ph; y++) {
            for (x = 0; x < pw; x++) {
                float cx = (x+0.5f) * s;
                float cy = (y+0.5f) * s;
                nvgCircle(vg, cx,cy, r);
            }
        }
        nvgFillColor(vg, nvgRGBA(220,160,0,200));
        nvgFill(vg);

        nvgEndFrame(vg);
        nvgluBindFramebuffer(NULL);
    }

    NVGLUframebuffer* m_fb = NULL;
};


int main(int argc, char const *argv[])
{
    Application::Config conf;
    conf.title = "NanoVG FBO Demo";
    conf.width = 1000;
    conf.height = 600;
    conf.msaa = 0;      // pick one
    conf.nvg_aa = true; // pick one
    FboDemo demo(conf);
    demo.run();
    return 0;
}
