// MIT License
//
// Copyright (c) Evan Pezent (epezent@rice.edu)
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

// ImPlot v0.1 WIP

#include <implot.h>
#include <imgui_internal.h>

namespace ImGui {
    

void ShowImPlotDemoWindow(bool* p_open) {

    ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(main_viewport_pos.x + 650, main_viewport_pos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImPlot Demo", p_open);
    ImGui::Text("implot says hello. (0.1 WIP)");
    if (ImGui::CollapsingHeader("Help")) {
        ImGui::Text("USER GUIDE:");
        ImGui::BulletText("Left click and drag within the plot area to pan X and Y axes.");
        ImGui::BulletText("Left click and drag on an axis to pan an individual axis.");
        ImGui::BulletText("Scroll in the plot area to zoom both X any Y axes");
        ImGui::BulletText("Scroll on an axis to zoom an individual axis.");
        ImGui::BulletText("Right click and drag to box select data.");
        ImGui::BulletText("Double left click to fit all visible data.");
        ImGui::BulletText("Double right click to open the plot context menu.");
        ImGui::BulletText("Click legend label icons to show/hide plot items.");
    }

    //-------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Line Plots")) {
        static float xs1[1001], ys1[1001];
        for (int i = 0; i < 1001; ++i) {
            xs1[i] = i * 0.001f;
            ys1[i] = 0.5f + 0.5f * sin(50 * xs1[i]);
        }
        static float xs2[11], ys2[11];
        for (int i = 0; i < 11; ++i) {
            xs2[i] = i * 0.1f;
            ys2[i] = xs2[i] * xs2[i];
        }
        if (ImGui::BeginPlot("Lines", "x", "f(x)", {-1,300})) {
            ImGui::Plot("sin(50*x)", xs1, ys1, 1001);
            ImGui::Plot("x^2", xs2, ys2, 11);
            ImGui::EndPlot();
        }
    }

    //-------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Scatter Plots")) {
        srand(0);
        static float xs1[100], ys1[100];
        for (int i = 0; i < 100; ++i) {
            xs1[i] = i * 0.01f;
            ys1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
        }
        static float xs2[50], ys2[50];
        for (int i = 0; i < 50; i++) {
            xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
            ys2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
        }
        if (ImGui::BeginPlot("Scatter", NULL, NULL, {-1,300})) {
            ImGui::PushPlotStyleVar(ImPlotStyleVar_LineWeight, 0);
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Cross);            
            ImGui::PushPlotStyleVar(ImPlotStyleVar_MarkerSize, 3);
            ImGui::Plot("Data 1", xs1, ys1, 100);
            ImGui::PopPlotStyleVar(2);
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Circle);
            ImGui::PushPlotColor(ImPlotCol_MarkerFill, ImVec4{1,0,0,0.25f});
            ImGui::Plot("Data 2", xs2, ys2, 50);
            ImGui::PopPlotColor();
            ImGui::PopPlotStyleVar(2);
            ImGui::EndPlot();
        }
    }

    //-------------------------------------------------------------------------
    if (ImGui::CollapsingHeader("Marker Styles")) {
        ImGui::SetNextPlotAxes(0, 10, 0, 12);
        if (ImGui::BeginPlot("##MarkerStyles", NULL, NULL, ImVec2(-1,300), 0, 0, 0)) {
            float xs[2] = {1,4};
            float ys[2] = {10,11};
            // filled
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Circle);
            ImGui::Plot("Circle##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Square);   ys[0]--; ys[1]--;
            ImGui::Plot("Square##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Diamond);  ys[0]--; ys[1]--;
            ImGui::Plot("Diamond##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Up);       ys[0]--; ys[1]--;
            ImGui::Plot("Up##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Down);     ys[0]--; ys[1]--;
            ImGui::Plot("Down##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Left);     ys[0]--; ys[1]--;
            ImGui::Plot("Left##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Right);    ys[0]--; ys[1]--;
            ImGui::Plot("Right##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Cross);    ys[0]--; ys[1]--;
            ImGui::Plot("Cross##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Plus);     ys[0]--; ys[1]--;
            ImGui::Plot("Plus##Fill", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Asterisk); ys[0]--; ys[1]--;
            ImGui::Plot("Asterisk##Fill", xs, ys, 2);   
            ImGui::PopPlotStyleVar(10);

            xs[0] = 6; xs[1] = 9;
            ys[0] = 10; ys[1] = 11;
            ImGui::PushPlotColor(ImPlotCol_MarkerFill, ImVec4(0,0,0,0));
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Circle);
            ImGui::Plot("Circle", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Square);   ys[0]--; ys[1]--;
            ImGui::Plot("Square", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Diamond);  ys[0]--; ys[1]--;
            ImGui::Plot("Diamond", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Up);       ys[0]--; ys[1]--;
            ImGui::Plot("Up", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Down);     ys[0]--; ys[1]--;
            ImGui::Plot("Down", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Left);     ys[0]--; ys[1]--;
            ImGui::Plot("Left", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Right);    ys[0]--; ys[1]--;
            ImGui::Plot("Right", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Cross);    ys[0]--; ys[1]--;
            ImGui::Plot("Cross", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Plus);     ys[0]--; ys[1]--;
            ImGui::Plot("Plus", xs, ys, 2);   
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Asterisk); ys[0]--; ys[1]--;
            ImGui::Plot("Asterisk", xs, ys, 2);   
            ImGui::PopPlotColor();
            ImGui::PopPlotStyleVar(10);

            xs[0] = 5; xs[1] = 5;
            ys[0] = 1; ys[1] = 11;

            ImGui::PushPlotStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImGui::PushPlotStyleVar(ImPlotStyleVar_MarkerSize, 8);
            ImGui::PushPlotStyleVar(ImPlotStyleVar_MarkerWeight, 2);
            ImGui::PushPlotStyleVar(ImPlotStyleVar_Marker, ImMarker_Circle | ImMarker_Cross); 
            ImGui::PushPlotColor(ImPlotCol_MarkerOutline, ImVec4(0,0,0,1));
            ImGui::PushPlotColor(ImPlotCol_MarkerFill, ImVec4(1,1,1,1));
            ImGui::PushPlotColor(ImPlotCol_Line, ImVec4(0,0,0,1));
            ImGui::Plot("Circle|Cross", xs, ys, 2);  
            ImGui::PopPlotStyleVar(4);  
            ImGui::PopPlotColor(3);

            ImGui::EndPlot();
        }
    }

    if (ImGui::CollapsingHeader("Log Scale")) {
        static float xs[1001], ys1[1001], ys2[1001], ys3[1001];
        for (int i = 0; i < 1001; ++i) {
            xs[i] = (float)(i*0.1f);
            ys1[i] = sin(xs[i]);
            ys2[i] = log(xs[i]);
            ys3[i] = pow(10, xs[i]);
        }
        ImGui::SetNextPlotAxes(0.1, 100, -1, 10);
        if (ImGui::BeginPlot("Log Plot", NULL, NULL, ImVec2(-1,300), ImPlotFlags_Default, ImAxisFlags_Default | ImAxisFlags_LogScale )) {
            ImGui::Plot("f(x) = x",      xs, xs,  1001);
            ImGui::Plot("f(x) = sin(x)", xs, ys1, 1001);
            ImGui::Plot("f(x) = log(x)", xs, ys2, 1001);
            ImGui::Plot("f(x) = 10^x",   xs, ys3, 21);
            ImGui::EndPlot();
        }
    }

    ImGui::End();
    
}

} // namespace ImGui