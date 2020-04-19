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

/// Special Color used to specific that a plot item color should set determined automatically.
#define IM_COL_AUTO ImVec4(0,0,0,-1)

typedef int ImPlotSpec;
typedef int ImPlotAxisFlags;
typedef int ImPlotFlags;
typedef int ImPlotCol;

enum ImPlotSpec_ {
    ImPlotSpec_SolidLine   = 1 << 0,  ///< solid lines will be rendered between consecutive points
    ImPlotSpec_Circle      = 1 << 1,  ///< a circle marker will be rendered at each point 
    ImPlotSpec_Square      = 1 << 2,  ///< a square maker will be rendered at each point
    ImPlotSpec_Diamond     = 1 << 3,  ///< a diamond marker will be rendered at each point
    ImPlotSpec_Up          = 1 << 4,  ///< an upward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Down        = 1 << 5,  ///< an downward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Left        = 1 << 6,  ///< an leftward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Right       = 1 << 7,  ///< an rightward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Cross       = 1 << 8,  ///< a cross marker will be rendered at each point (no fill)
    ImPlotSpec_Plus        = 1 << 9,  ///< a plus marker will be rendered at each point (no fill)
    ImPlotSpec_Asterisk    = 1 << 10, ///< a asterisk marker will be rendered at each point (no fill)
    ImPlotSpec_AntiAliased = 1 << 11  ///< lines and fills will be anti-aliased (not recommended)
};

enum ImPlotAxisFlags_ {
    ImPlotAxisFlags_GridLines  = 1 << 0, ///< grid lines will be displayed
    ImPlotAxisFlags_TickMarks  = 1 << 1, ///< tick marks will be displayed for each grid line
    ImPlotAxisFlags_TickLabels = 1 << 2, ///< text labels will be displayed for each grid line
    ImPlotAxisFlags_Flip       = 1 << 3, ///< the axis will be flipped
    ImPlotAxisFlags_LockMin    = 1 << 4, ///< the axis minimum value will be locked when panning/zooming
    ImPlotAxisFlags_LockMax    = 1 << 5, ///< the axis maximum value will be locked when panning/zooming
    ImPlotAxisFlags_Adaptive   = 1 << 6, ///< grid divisions will adapt to the current pixel size the axis
    ImPlotAxisFlags_LogScale   = 1 << 7, ///< a logartithmic axis scale will be used (TODO)
    ImPlotAxisFlags_Scientific = 1 << 8, ///< scientific notation will be used for tick labels if displayed
    ImPlotAxisFlags_Default    = ImPlotAxisFlags_GridLines | ImPlotAxisFlags_TickMarks | ImPlotAxisFlags_TickLabels | ImPlotAxisFlags_Adaptive
};

enum ImPlotFlags_ {
    ImPlotFlags_MousePos    = 1 << 0, ///< the mouse position, in plot coordinates, will be displayed in the bottom-right
    ImPlotFlags_Legend      = 1 << 1, ///< a legend will be displayed in the top-left
    ImPlotFlags_Selection   = 1 << 2, ///< the user will be able to box-select with right-mouse
    ImPlotFlags_ContextMenu = 1 << 3, ///< the user will be able to open a context menu with double-right click
    ImPlotFlags_Crosshairs  = 1 << 4, ///< the default mouse cursor will be replaced with a crosshair
    ImPlotFlags_Default     = ImPlotFlags_MousePos | ImPlotFlags_Legend | ImPlotFlags_Selection | ImPlotFlags_ContextMenu
};

enum ImPlotCol_ {
    ImPlotCol_FrameBg,
    ImPlotCol_PlotBg,
    ImPlotCol_PlotBorder,
    ImPlotCol_Selection
};

struct ImPlotAxis {
    ImPlotAxis() { Min = 0; Max = 1; Divisions = 3; Subdivisions = 10; Col = IM_COL_AUTO; Flags = ImPlotAxisFlags_Default; }
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

void PlotLine(const char* label, const float* xs, const float* ys, int size, ImPlotSpec spec = ImPlotSpec_SolidLine, 
              const ImVec4& color_line = IM_COL_AUTO, const ImVec4& color_fill = ImVec4(0,0,0,0),
              int offset = 0, int stride = 1, bool cull = true);

void PlotLabel();
void PlotBar();
void PlotStem();
void PlotErrorBars();


void PushPlotColor(ImPlotCol idx, const ImVec4& col);
void PushPlotColor(ImPlotCol idx, ImU32 col);
void PopPlotColor();

}  // namespace ImGui