#include <mahi/gui.hpp>

using namespace mahi::gui;

class Integrator {
public:
    Integrator(double initial = 0.0) : integral(initial) { }
    double update(double x, double t) {
        integral += (t - last_t) * (0.5 * (last_x + x));
        last_x = x;
        last_t = t;
        return integral;
    }
private:
    double last_x;  ///< Integrand at previous call to integrate()
    double last_t;  ///< Time at previous call to integrate()
    double integral; ///< The integral value
};

// Simulated system to control
class MassSpringDamper {
public:
    // Constructor
    MassSpringDamper(double x0 = 0, double xd0 = 0) :
        x(x0), xd(xd0), xdd(0), xdd_xd(xd), xd_x(x)
    { }
    // Propogate MSD system
    void update(double F, double t) {
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
    double k = 20; // spring, N/m
    double b = 30; // damping, Ns/m
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
    MsdDemo() : Application(500,500,"MSD Demo") { 
        // ImGui::DisableViewports();
    }

    void update() override {

        ImGui::Begin("Mass Spring Damper");
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
        double t = 0;
        double minx = Math::INF;
        double maxx = -Math::INF;
        for (int i = 0 ; i < steps; ++i) {
            msd.update(0, t);
            xplot.data.push_back(Vec2(t, msd.x));
            minx = std::min(minx, msd.x);
            maxx = std::max(maxx, msd.x);
            t += tstep;
        }
        plot.xAxis.minimum = 0;
        plot.xAxis.maximum = tmax;
        plot.yAxis.minimum = minx;
        plot.yAxis.maximum = maxx;
    }

    MassSpringDamper msd;
    double x0 = 0, xd0 = 0;
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
