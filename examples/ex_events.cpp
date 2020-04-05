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

/// you can prevent the Window from closing by returning false
bool window_close_handler() {
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
        on_window_moved.connect( [](int x, int y) { print_var(x,y); } );
        // connect Event using non-static member function
        on_window_resized.connect(this, &EventsDemo::window_resize_handler); 
        // connect Event usign a static member function
        on_file_drop.connect(&EventsDemo::file_drop_handler);
        // connect Event using a free function
        on_window_closing.connect(window_close_handler);
    }

    void window_resize_handler(int width, int height) {
        print_var(width, height);
    }

    static void file_drop_handler(const std::vector<std::string>& paths) {
        for (auto& p : paths) 
            print_var(p);
    }
};

int main(int argc, char const *argv[])
{
    EventsDemo demo;
    demo.run();
    return 0;
}
