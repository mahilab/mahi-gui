#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <thread>

// create global stop variable CTRL-C handler function
mahi::util::ctrl_bool stop(false);
bool handler(mahi::util::CtrlEvent event) {
    stop = true;
    return true;
}

int main(){
    // initialize plot helper and 
    mahi::gui::PlotHelper plot_helper;
    mahi::gui::run_plotter(plot_helper);

    // set loop rate to once every 1ms (1000Hz)
    mahi::util::Timer timer(mahi::util::milliseconds(1));
    mahi::util::Time t = mahi::util::Time::Zero;

    // only stop with global handler
    while (!stop){
        // create data
        double sin_data = mahi::util::sin(t.as_seconds() * 2 * mahi::util::PI);
        double cos_data = mahi::util::cos(t.as_seconds() * 2 * mahi::util::PI);

        // add data to be plotted like this.
        // this should be called once per loop
        plot_helper.add_data("sin", sin_data);
        plot_helper.add_data("cos", cos_data);

        // No data is actually sent until yuo run this line below
        plot_helper.write_data();

        // force loop to 1ms loop rate
        t = timer.wait();
    }

    return 0;
}
