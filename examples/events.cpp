#include <mahi/gui.hpp>

using namespace mahi::gui;

/// you can prevent the Window from closing by returning false
bool windowCloseHandler() {
    static bool firstAttempt = true;
    if (firstAttempt) {
        print("Not going to let you close the window that easily! Try again ...");
        firstAttempt = false;
        return false;
    }
    return true;
}

class EventsDemo : public Application {
public:
    EventsDemo() : Application(500,500,"Events Demo") {     
        // connect Event using a lambda:
        onWindowMoved.connect( [](int x, int y) { print(x,y); } );
        // connect Event using non-static member function
        onWindowResized.connect(this, &EventsDemo::windowResizeHandler); 
        // connect Event usign a static member function
        onFileDrop.connect(&EventsDemo::fileDropHandler);
        // connect Event using a free function
        onWindowClosed.connect(windowCloseHandler);
    }

    void windowResizeHandler(int width, int height) {
        print(width, height);
    }

    static void fileDropHandler(const std::vector<std::string>& paths) {
        for (auto& p : paths) 
            print(p);
    }
};

int main(int argc, char const *argv[])
{
    EventsDemo demo;
    demo.run();
    return 0;
}
