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

#define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;

//=============================================================================
// MAHI LOG WRITER
//============================================================================

template <class Formatter>
class GuiLogWritter : public Writer {
public:
    GuiLogWritter(Severity max_severity = Debug) : Writer(max_severity), logs(500) {}

    virtual void write(const LogRecord& record) override {
        auto log = std::pair<Severity, std::string>(record.get_severity(), Formatter::format(record));
        logs.push_back(log);
    }
    RingBuffer<std::pair<Severity, std::string>> logs;
};

static GuiLogWritter<TxtFormatter> writer;

//=============================================================================
// LOG WINDOW
//============================================================================

class LogWindowDemo : public Application
{
public:

    ImGuiTextFilter filter;
    std::unordered_map<Severity, Color> colors = {
        {None    , Grays::Gray50},  
        {Fatal   , Reds::Crimson}, 
        {Error   , Pinks::HotPink}, 
        {Warning , Yellows::Yellow}, 
        {Info    , Whites::White}, 
        {Verbose , Greens::LightGreen}, 
        {Debug   , Cyans::Cyan}
    };

    LogWindowDemo() {
        // add writer to MAHI_LOG
        if (MahiLogger) {
            MahiLogger->add_writer(&writer);
            MahiLogger->set_max_severity(Debug);
        }
    }

    void update()
    {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("MAHI Log", &open);
        for (int i = 0; i < 7; ++i) {

            if (ImGui::Button(severity_to_string((Severity)i)))  {
                LOG((Severity)i) << "Here's a " << severity_to_string((Severity)i) << " log";
            }

            if (i != 6)
                ImGui::SameLine();
        }
        ImGui::Separator();

        if (ImGui::Button("Clear"))
            writer.logs.clear();
        ImGui::SameLine();
        filter.Draw("Filter",-50);
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (unsigned int i = 0; i < writer.logs.size(); ++i)
        {
            if (filter.PassFilter(writer.logs[i].second.c_str())) {
                ImGui::PushStyleColor(ImGuiCol_Text, colors[writer.logs[i].first]);
                ImGui::TextUnformatted(writer.logs[i].second.c_str());
                ImGui::PopStyleColor();
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
        ImGui::End();
        
        if (!open)
            quit();
    }

    bool open = true;
};

//=============================================================================
// MAIN
//============================================================================

int main(int argc, char const *argv[])
{
    LogWindowDemo demo;
    demo.run();
    return 0;
}
