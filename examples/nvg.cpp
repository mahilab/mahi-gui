#include <mahi/gui.hpp>

using namespace mahi::gui;

class NvgDemo : public Application {
public:
    NvgDemo() : Application(250,250,"NVG Demo") { 
        backgroundColor = Grays::Gray10;
        enableVSync(false);
    }
    void update() override { 

        ImGui::Begin("ImGui");
        ImGui::Text("ImGui will always be drawn on top of NanoVG");
        ImGui::Text("even if it is called first in update()");
        if (ImGui::Button("VSync On"))
            enableVSync(true);
        ImGui::SameLine();
        if (ImGui::Button("VSync Off"))
            enableVSync(false);
        ImGui::Text("%.3f FPS", ImGui::GetIO().Framerate);
        ImGui::End();

        auto [w,h] = getWindowSize();
        auto [x,y] = getMousePosition();
        double t = time();

        drawEyes(vg, (w-150)/2, (h-100)/2, 150, 100, x, y, t);

        nvgBeginPath(vg);
        nvgCircle(vg, x, y, 5);
        nvgFillColor(vg, nvgRGBA(255,192,0,255));
        nvgFill(vg);
    }

    // taken from 3rdparty/nanovg/examples/demo.c
    void drawEyes(NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
    {
        NVGpaint gloss, bg;
        float ex = w *0.23f;
        float ey = h * 0.5f;
        float lx = x + ex;
        float ly = y + ey;
        float rx = x + w - ex;
        float ry = y + ey;
        float dx,dy,d;
        float br = (ex < ey ? ex : ey) * 0.5f;
        float blink = 1 - pow(sinf(t*0.5f),200)*0.8f;

        bg = nvgLinearGradient(vg, x,y+h*0.5f,x+w*0.1f,y+h, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,16));
        nvgBeginPath(vg);
        nvgEllipse(vg, lx+3.0f,ly+16.0f, ex,ey);
        nvgEllipse(vg, rx+3.0f,ry+16.0f, ex,ey);
        nvgFillPaint(vg, bg);
        nvgFill(vg);

        bg = nvgLinearGradient(vg, x,y+h*0.25f,x+w*0.1f,y+h, nvgRGBA(220,220,220,255), nvgRGBA(128,128,128,255));
        nvgBeginPath(vg);
        nvgEllipse(vg, lx,ly, ex,ey);
        nvgEllipse(vg, rx,ry, ex,ey);
        nvgFillPaint(vg, bg);
        nvgFill(vg);

        dx = (mx - rx) / (ex * 10);
        dy = (my - ry) / (ey * 10);
        d = sqrtf(dx*dx+dy*dy);
        if (d > 1.0f) {
            dx /= d; dy /= d;
        }
        dx *= ex*0.4f;
        dy *= ey*0.5f;
        nvgBeginPath(vg);
        nvgEllipse(vg, lx+dx,ly+dy+ey*0.25f*(1-blink), br,br*blink);
        nvgFillColor(vg, nvgRGBA(32,32,32,255));
        nvgFill(vg);

        dx = (mx - rx) / (ex * 10);
        dy = (my - ry) / (ey * 10);
        d = sqrtf(dx*dx+dy*dy);
        if (d > 1.0f) {
            dx /= d; dy /= d;
        }
        dx *= ex*0.4f;
        dy *= ey*0.5f;
        nvgBeginPath(vg);
        nvgEllipse(vg, rx+dx,ry+dy+ey*0.25f*(1-blink), br,br*blink);
        nvgFillColor(vg, nvgRGBA(32,32,32,255));
        nvgFill(vg);

        gloss = nvgRadialGradient(vg, lx-ex*0.25f,ly-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
        nvgBeginPath(vg);
        nvgEllipse(vg, lx,ly, ex,ey);
        nvgFillPaint(vg, gloss);
        nvgFill(vg);

        gloss = nvgRadialGradient(vg, rx-ex*0.25f,ry-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
        nvgBeginPath(vg);
        nvgEllipse(vg, rx,ry, ex,ey);
        nvgFillPaint(vg, gloss);
        nvgFill(vg);
    }
};

int main(int argc, char const *argv[])
{
    NvgDemo demo;
    demo.run();
    return 0;
}
