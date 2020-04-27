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

// Simulated system to control
class MassSpringDamper {
public:
    // Constructor
    MassSpringDamper(double x0 = 0, double xd0 = 0) :
        x(x0), xd(xd0), xdd(0), xdd_xd(xd), xd_x(x)
    { }
    // Propogate MSD system
    void update(double F, Time t) {
        xdd = (F - b * xd - k * x) / m;
        xd  = xdd_xd.update(xdd, t); 
        x   = xd_x.update(xd, t);
    }
    void reset() {
        xd_x = Integrator(x);
        xdd_xd = Integrator(xd);
    }
    // Properties
    double m = 10; // mass, kg    
    double k = 50; // spring, N/m
    double b = 10; // damping, Ns/m
    // State
    double x; // position, m
    double xd; // velocity, m/s
    double xdd; // acceleration, m/s^2
private:
    // Integrators
    Integrator xdd_xd, xd_x;    
};

class MsdDemo : public Application {
public:
    MsdDemo() : Application(1000,500,"MSD Demo") { 
        ImGui::DisableViewports();
    }

    void update() override {
        ImGui::BeginFixed("Mass Spring Damper",{10,10}, {480,480}, ImGuiWindowFlags_NoTitleBar);
        ImGui::DragDouble("M",&msd.m,1,0,100,"%.1f kg");
        ImGui::DragDouble("K",&msd.k,1,0,100,"%.1f N/m");
        ImGui::DragDouble("B",&msd.b,1,0,100,"%.1f Ns/m");
        ImGui::DragDouble("X0", &x0, 0.1f, -10, 10);
        ImGui::DragDouble("T Max",&tmax, 1, 0, 100, "%.0f s");
        if (ImGui::Button("Simulate"))
            simulate();
        ImGui::Plot("Plot", &plot, &xplot, 1);
        ImGui::End();
    }

    void simulate() {
        msd.x = x0; msd.xd = xd0;
        msd.reset();
        xplot.data.clear();
        xplot.data.reserve(steps);
        double tstep = tmax / (steps - 1);
        Time t = Time::Zero;
        double minx = INF;
        double maxx = -INF;
        for (int i = 0 ; i < steps; ++i) {
            msd.update(0, t);
            xplot.data.push_back(Vec2(t.as_seconds(), msd.x));
            minx = std::min(minx, msd.x);
            maxx = std::max(maxx, msd.x);
            t += seconds(tstep);
        }
        plot.x_axis.minimum = 0;
        plot.x_axis.maximum = tmax;
        plot.y_axis.minimum = minx;
        plot.y_axis.maximum = maxx;
    }

    void drawMsd() {
        
    }

    MassSpringDamper msd;
    double x0 = 5, xd0 = 0;
    ImGui::PlotInterface plot;
    ImGui::PlotItem xplot;
    double tmax = 10;
    int steps = 10000;
};

int main(int argc, char const *argv[])
{
    MsdDemo demo;
    demo.run();
    return 0;
}
