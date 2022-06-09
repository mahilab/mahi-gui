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

    /// general GUI variables
    std::string     title_;                 // window title
    float           width_{1920};           // window width
    float           height_{1080};          // window height
    bool            loaded_{false};         // was the text window config loaded?

    /// font options from MAHI GUI
    enum FontOptions {
        Roboto_Regular = 0,
        Roboto_Bold = 1,
        Roboto_Italic = 2,
        RobotoMono_Regular = 3,
        RobotoMono_Bold = 4,     // Note that RobotMono_Bold is imported by mahi-gui by default and is only included here for clarity
        RobotoMono_Italic = 5
    };

    /// text variables
    std::vector<std::vector<ImFont*>>   fonts_;     // vector of font pointers
    std::vector<int>                    fontsize_;  // vector holding fontsize options
    int currentSizeIndex_{0};                       // default fontsize index
    FontOptions currentFont_{RobotoMono_Bold};      // default font
    std::string dummyText_ = 
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "In ut malesuada magna. Proin et iaculis felis, nec faucibus nisi. "
        "Sed gravida tempor risus ac accumsan. Pellentesque placerat faucibus eros sed fringilla. "
        "Nam eu euismod orci, at vestibulum risus. Pellentesque ut aliquam diam, eget maximus urna. "
        "Suspendisse potenti. Curabitur tempus porttitor neque non sodales. Praesent ac rutrum nisl. "
        "Nam eleifend nisi et ligula fringilla iaculis id ac arcu. In ante justo, suscipit ac dui vel, feugiat viverra elit. "
        "Fusce auctor magna vitae justo scelerisque rhoncus. Fusce vel feugiat odio.";


    // base methods
    //-----------------------------------
    /// constructor
    TextWindow() : Application(500,500,"",false) 
    { 
        // load config
        loaded_ = load();

        // ImGui
        ImGui::DisableViewports();
        ImGui::DisableDocking();
        ImPlot::StyleColorsDark();
    }

    /// main update code
    void update() override {
        ImGui::BeginFixed("##TextWindow", ImVec2{0,0}, ImVec2{1000, 800}, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
        if (loaded_) 
        {
            // show text size options
            for (unsigned int i = 0; i < fontsize_.size(); ++i) 
            {            
                if (ImGui::RadioButton(std::to_string(fontsize_[i]).c_str(), currentSizeIndex_ == i) && currentSizeIndex_ != i) 
                    currentSizeIndex_ = i;
                if(i+1 != fontsize_.size()) ImGui::SameLine();
            }     
            // show font type options
            {
                if (ImGui::RadioButton("Roboto Regular", currentFont_ == Roboto_Regular) && currentFont_ != Roboto_Regular) 
                    currentFont_ = Roboto_Regular;
                ImGui::SameLine();
                if (ImGui::RadioButton("Roboto Bold", currentFont_ == Roboto_Bold) && currentFont_ != Roboto_Bold) 
                    currentFont_ = Roboto_Bold;
                ImGui::SameLine();
                if (ImGui::RadioButton("Roboto Italic", currentFont_ == Roboto_Italic) && currentFont_ != Roboto_Italic) 
                    currentFont_ = Roboto_Italic;
                ImGui::SameLine();
                if (ImGui::RadioButton("RobotoMono Regular", currentFont_ == RobotoMono_Regular) && currentFont_ != RobotoMono_Regular) 
                    currentFont_ = RobotoMono_Regular;
                ImGui::SameLine();
                if (ImGui::RadioButton("RobotoMono Bold", currentFont_ == RobotoMono_Bold) && currentFont_ != RobotoMono_Bold) 
                    currentFont_ = RobotoMono_Bold;
                ImGui::SameLine();
                if (ImGui::RadioButton("RobotoMono Italic", currentFont_ == RobotoMono_Italic) && currentFont_ != RobotoMono_Italic) 
                    currentFont_ = RobotoMono_Italic;
            }     
            ImGui::Separator();
            ImGui::Separator();

            // change font used
            ImGui::PushFont(fonts_[(int)currentFont_][currentSizeIndex_]);      

            // Display dummy text
            ImGui::TextWrapped(dummyText_.c_str());

            // stop use of font
            ImGui::PopFont();
        }
        else {
            ImGui::Text("Text window failed to load! :(");
        }
        ImGui::End();
    }

    /// Load in text window config file
    bool load() {
        // checks for config file
        if (fs::exists("font_config.json")) {
            // load in config settings
            try {
                // open and load in config file
                std::ifstream file("font_config.json");
                json j;
                file >> j;
                
                // convert relevant variables for use in GUI
                title_      = j["title"].get<std::string>();
                width_      = j["width"].get<float>();
                height_     = j["height"].get<float>();
                fontsize_   = j["fontsize"].get<std::vector<int>>();
                currentSizeIndex_ = std::floor(fontsize_.size() / 2);

                // set window title and size
                set_window_title(title_);
                set_window_size((int)width_, (int)height_);
                center_window();
                
                // load in all font options                
                {
                    ImGuiIO& io     =   ImGui::GetIO();
                    std::vector<ImFont*> fontset;

                    // Roboto Regular Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();

                    // Roboto Bold Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(Roboto_Bold_ttf, Roboto_Bold_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();

                    // Roboto Italic Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(Roboto_Italic_ttf, Roboto_Italic_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();

                    // RobotoMono Regular Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(RobotoMono_Regular_ttf, RobotoMono_Regular_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();

                    // RobotoMono Bold Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();

                    // RobotoMono Italic Set
                    for(unsigned int i = 0; i < fontsize_.size(); i++)
                        fontset.push_back(io.Fonts->AddFontFromMemoryTTF(RobotoMono_Italic_ttf, RobotoMono_Italic_ttf_len, fontsize_[i]));
                    fonts_.push_back(fontset);
                    fontset.clear();
                }

                // add information to mahi log
                LOG(Info) << "Test Config file loaded";
            }
            // note if loading fails
            catch(...) {
                // add information to mahi log
                LOG(Fatal) << "Test Config file could not be loaded";

                // return error
                return false;
            }
            return true;
        }
        return false;
    }
};

int main(int argc, char const *argv[])
{
    // if there doesn't exist a "font_config.json" file, make a default one
    if (!fs::exists("font_config.json")) {
        json j;
        j["title"]      =   "Font Example";
        j["width"]      =   1000;
        j["height"]     =   800;
        j["fontsize"]   =   {8,10,12,14,16,20,24,28,36,48,72,100};
        std::ofstream file("font_config.json");
        if (file.is_open())
            file << std::setw(4) << j;
    }
    // run the GUI
    TextWindow textWindow;
    textWindow.run();
    return 0;
}
