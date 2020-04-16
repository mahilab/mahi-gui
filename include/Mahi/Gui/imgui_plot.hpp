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

#pragma once
#include <imgui.h>
#include <vector>
#include <string>

#define IM_AUTO_COLOR ImVec4(0,0,0,-1)

typedef int ImPlotSpec;
typedef int ImPlotCol;
typedef int ImPlotFlags;
typedef int ImPlotAxisFlags;

enum ImPlotSpec_ {
    ImPlotSpec_Solid    = 1 << 0,
    ImPlotSpec_Asterisk = 1 << 1,
    ImPlotSpec_Circle   = 1 << 2,
    ImPlotSpec_Cross    = 1 << 3,
    ImPlotSpec_Square   = 1 << 5,
    ImPlotSpec_Diamond  = 1 << 6,
    ImPlotSpec_Up       = 1 << 7,
    ImPlotSpec_Down     = 1 << 8,
    ImPlotSpec_Left     = 1 << 9,
    ImPlotSpec_Right    = 1 << 10
};

enum ImPlotCol_ {
    ImPlotCol_FrameBg,
    ImPlotCol_PlotBg,
    ImPlotCol_PlotBorder,
    ImPlotCol_Selection
};

enum ImPlotFlags_ {
    ImPlotFlags_CursorLabel = 1 << 0,
    ImPlotFlags_Legend      = 1 << 1,
    ImPlotFlags_Selection   = 1 << 2,
    ImPlotFlags_ContextMenu = 1 << 3,
    ImPlotFlags_Crosshairs  = 1 << 4,
    ImPlotFlags_Default     = ImPlotFlags_CursorLabel | ImPlotFlags_Legend | ImPlotFlags_Selection | ImPlotFlags_ContextMenu
};

enum ImPlotAxisFlags_ {
    ImPlotAxisFlags_Grid       = 1 << 0,
    ImPlotAxisFlags_TickMarks  = 1 << 1,
    ImPlotAxisFlags_TickLabels = 1 << 2,
    ImPlotAxisFlags_Flip       = 1 << 3,
    ImPlotAxisFlags_LockMin    = 1 << 4,
    ImPlotAxisFlags_LockMax    = 1 << 5,
    ImPlotAxisFlags_Adaptive   = 1 << 6,
    ImPlotAxisFlags_Default    = ImPlotAxisFlags_Grid | ImPlotAxisFlags_TickMarks | ImPlotAxisFlags_TickLabels | ImPlotAxisFlags_Adaptive
};

struct ImPlotAxis {
    ImPlotAxis() { Min = 0; Max = 1; Divisions = 3; Subdivisions = 10; Col = IM_AUTO_COLOR; Flags = ImPlotAxisFlags_Default; }
    float Min;
    float Max;
    int Divisions;
    int Subdivisions;
    ImVec4 Col;
    ImPlotAxisFlags Flags;
};

struct ImPlot {
    ImPlot() { Flags = ImPlotFlags_Default; }
    ImPlotAxis XAxis;
    ImPlotAxis YAxis;
    ImPlotFlags Flags;
};

namespace ImGui {

bool BeginPlot(const char* title, ImPlot* plot, 
               const char* x_label = nullptr, const char* y_label = nullptr, 
               const ImVec2& size = {-1, -1});

void EndPlot();

void PlotLine(const char* label, const float* xs, const float* ys, int size, ImPlotSpec spec = ImPlotSpec_Solid, 
              const ImVec4& color_line = {1,1,1,1}, const ImVec4& color_fill = {0,0,0,0},
              bool* show = nullptr, int offset = 0, int stride = 1);

void PlotLabel();
void PlotBar();
void PlotErrorBars();


void PushPlotColor(ImPlotCol idx, const ImVec4& col);
void PushPlotColor(ImPlotCol idx, ImU32 col);
void PopPlotColor();

}  // namespace ImGui