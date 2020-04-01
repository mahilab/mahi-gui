#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class ShapeDemo : public Application {
public:
    ShapeDemo(const Config& cfg) : Application(cfg) { 
        shape = make_polygon_shape(5, 50);
        shape.set_radii(10);
        shape.move(100,100);
        shape2 = offset_shape(shape, 20);

        auto s1 = make_rectangle_shape(50,50);
        s1.move(250,50);
        auto s2 = s1;
        s2.move(25,25);
        union_shape = clip_shapes(s1,s2,ClipType::Union);

        s1.move(100,0);
        s2.move(100,0);
        diff_shape = clip_shapes(s1,s2,ClipType::Difference);

        s1.move(-100,100);
        s2.move(-100,100);
        excl_shape = clip_shapes(s1,s2,ClipType::Exclusion);

        s1.move(100,0);
        s2.move(100,0);
        ints_shape = clip_shapes(s1,s2,ClipType::Intersection);

        star = make_star_shape(5, 15, 30);
        star.move(100,100);
        star.set_radii(2);

        hole_shape = make_polygon_shape(6, 50);
        hole_shape.set_hole_count(1);
        hole_shape.set_hole(0, make_star_shape(6, 30, 20));
        
    }
    void draw(NVGcontext* nvg) override {  
        nvgFillShape(nvg, shape2, Greens::ForestGreen);
        nvgFillShape(nvg, shape,  Greens::Chartreuse);
        nvgFillShape(nvg, star,   Greens::ForestGreen);
        for (auto& s : union_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : diff_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : excl_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : ints_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);

        nvgTranslate(nvg, 100, 400);
        nvgFillShape(nvg, hole_shape, Blues::DeepSkyBlue);
        nvgStrokeShape(nvg, hole_shape, 5, Whites::Azure);
        nvgResetTransform(nvg);
    } 

    Shape shape;
    Shape shape2;
    std::vector<Shape> union_shape;
    std::vector<Shape> diff_shape;
    std::vector<Shape> excl_shape;
    std::vector<Shape> ints_shape;
    Shape hole_shape;
    Shape star;
};

int main(int argc, char const *argv[])
{
    Application::Config cfg;
    cfg.msaa = 0;
    ShapeDemo demo(cfg);
    demo.run();
    return 0;
}
