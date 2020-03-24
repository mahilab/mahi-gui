#include <Mahi/Gui.hpp>
#include <mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

void prepare_path(NVGcontext* vg, const Shape& shape) {
    const auto& vertices = shape.vertices();
    nvgMoveTo(vg, vertices[0].x, vertices[0].y);
    for (int i = 1; i < vertices.size(); ++i) {
        nvgMoveTo(vg, vertices[i].x, vertices[i].y);
    }
    nvgClosePath(vg);
}

class ShapeDemo : public Application {
public:
    ShapeDemo() : Application(500,500,"Shape Demo") { 
        background_color = Grays::Gray10;
        shape.push_back(10,10);
        shape.push_back(10,490);
        shape.push_back(490,490);
        shape.push_back(490,10);
    }

    void update() override {  

        nvgBeginPath(vg);
        prepare_path(vg, shape);
        nvgFillColor(vg, nvgRGBAf(1,1,0,1));
        nvgFill(vg);
    } 

    Shape shape;

};

int main(int argc, char const *argv[])
{
    ShapeDemo demo;
    demo.run();
    return 0;
}
