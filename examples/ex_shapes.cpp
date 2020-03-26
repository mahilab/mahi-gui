#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

class ShapeDemo : public Application {
public:
    ShapeDemo() : Application("Shape Demo") { 
        background_color = Grays::Gray10;
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
        
    }
    void update() override {  

        fill_shape(vg, shape2, Greens::ForestGreen);
        fill_shape(vg, shape,  Greens::Chartreuse);
        fill_shape(vg, star,   Greens::ForestGreen);

        for (auto& s : union_shape)
            fill_shape(vg, s, Reds::FireBrick);
        for (auto& s : diff_shape)
            fill_shape(vg, s, Reds::FireBrick);
        for (auto& s : excl_shape)
            fill_shape(vg, s, Reds::FireBrick);
        for (auto& s : ints_shape)
            fill_shape(vg, s, Reds::FireBrick);
    } 

    Shape shape;
    Shape shape2;
    std::vector<Shape> union_shape;
    std::vector<Shape> diff_shape;
    std::vector<Shape> excl_shape;
    std::vector<Shape> ints_shape;
    Shape star;
};

int main(int argc, char const *argv[])
{
    ShapeDemo demo;
    demo.run();
    return 0;
}
