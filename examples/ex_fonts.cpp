// MIT License
//
// Copyright (c) 2022 Mechatronics and Haptic Interfaces Lab - Rice University
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
// Author(s): Zane Zook (gadzooks@rice.edu)

//-----------------------------------------------------------------------------
// preprocessor directives
//-----------------------------------------------------------------------------
#define MAHI_GUI_NO_CONSOLE

/// mahi includes
#include <Mahi/Gui.hpp>         // mahi-gui include
#include <Mahi/Util.hpp>        // mahi-utility include

// definition of filesystem import based on os
#if defined(__linux__)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

// relevant MAHI namespaces to use
using namespace mahi::gui;
using namespace mahi::util;

//-----------------------------------------------------------------------------
// text window class declaration
//-----------------------------------------------------------------------------
class TextWindow : public Application {
public:
    // public variables
    //-----------------------------------
    std::string dummyText = "The quick brown fox jumps over the lazy dog";

    // base methods
    //-----------------------------------
    /// constructor
    TextWindow() : Application(500,500,"",false) { 
        ImGui::DisableViewports();
        loaded = load();
    }

    /// main update code
    void update() override {
        ImGui::BeginFixed("##TextWindow", ImVec2{0,0}, ImVec2{1000, 800}, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        if (loaded) 
        {
            // Display dummy text
            ImGui::TextWrapped(dummyText.c_str());
        }
        else {
            ImGui::Text("Text window failed to load! :(");
        }
        ImGui::End();
    }

    /// Load in Likert config file
    bool load() {
        if (fs::exists("likert.json")) {
            try {
                std::ifstream file("likert.json");
                json j;
                file >> j;
                title = j["title"].get<std::string>();
                // set_window_title(title);
                // set_window_size((int)width, (int)height);
                center_window();
            }
            catch(...) {
                return false;
            }
            return true;
        }
        return false;
    }

    bool loaded = false;                 ///< was the text window config loaded?
    std::string title;                   ///< survey title
    float width, height;                 ///< window width/height
};

int main(int argc, char const *argv[])
{
    // if there doesn't exist a "tw_config.json" file, make a default one
    if (!fs::exists("tw_config.json")) {
        json j;
        j["title"] = "My Likert Survey";
        std::ofstream file("likert.json");
        if (file.is_open())
            file << std::setw(4) << j;
    }
    // run the GUI
    TextWindow textWindow;
    textWindow.run();
    return 0;
}
