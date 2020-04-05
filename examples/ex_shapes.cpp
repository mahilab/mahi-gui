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

class ShapeDemo : public Application {
public:
    ShapeDemo(const Config& cfg) : Application(cfg) { 

        shape = make_polygon_shape(5, 50);
        shape.set_radii(10);
        shape2 = offset_shape(shape, 20);
        star = make_star_shape(5, 15, 30);
        star.set_radii(2);

        hole_shape = make_polygon_shape(6, 50);
        hole_shape.set_hole_count(1);
        hole_shape.set_hole(0, make_star_shape(6, 30, 20));

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
    }
    
    void draw(NVGcontext* nvg) override {  

        nvgTranslate(nvg, 100, 100);
        nvgFillShape(nvg, shape2, Greens::ForestGreen);
        nvgFillShape(nvg, shape,  Greens::Chartreuse);
        nvgFillShape(nvg, star,   Greens::ForestGreen);
        nvgResetTransform(nvg);

        nvgTranslate(nvg, 100, 400);
        nvgFillShape(nvg, hole_shape, Blues::DeepSkyBlue);
        nvgStrokeShape(nvg, hole_shape, 5, Whites::Azure);
        nvgResetTransform(nvg);

        for (auto& s : union_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : diff_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : excl_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
        for (auto& s : ints_shape)
            nvgFillShape(nvg, s, Reds::FireBrick);
    } 

    Shape shape;
    Shape shape2;
    Shape star;
    Shape hole_shape;

    std::vector<Shape> union_shape;
    std::vector<Shape> diff_shape;
    std::vector<Shape> excl_shape;
    std::vector<Shape> ints_shape;
};

int main(int argc, char const *argv[])
{
    Application::Config cfg;
    cfg.msaa = 0;
    ShapeDemo demo(cfg);
    demo.run();
    return 0;
}
