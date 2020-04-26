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

#pragma once
#include <imgui.h>

//-----------------------------------------------------------------------------
// Basic types and flags
//-----------------------------------------------------------------------------

typedef int ImPlotFlags;
typedef int ImPlotAxisFlags;
typedef int ImPlotCol;
typedef int ImPlotStyleVar;
typedef int ImPlotMarker;

// Options for plots
enum ImPlotFlags_ {
    ImPlotFlags_MousePos    = 1 << 0, // the mouse position, in plot coordinates, will be displayed in the bottom-right
    ImPlotFlags_Legend      = 1 << 1, // a legend will be displayed in the top-left
    ImPlotFlags_Selection   = 1 << 2, // the user will be able to box-select with right-mouse
    ImPlotFlags_ContextMenu = 1 << 3, // the user will be able to open a context menu with double-right click
    ImPlotFlags_Crosshairs  = 1 << 4, // the default mouse cursor will be replaced with a crosshair when hovered
    ImPlotFlags_CullData    = 1 << 5, // plot data outside the plot area will be culled from rendering
    ImPlotFlags_AntiAliased = 1 << 6, // lines and fills will be anti-aliased (not recommended)
    ImPlotFlags_Default     = ImPlotFlags_MousePos | ImPlotFlags_Legend | ImPlotFlags_Selection | ImPlotFlags_ContextMenu | ImPlotFlags_CullData
};

// Options for plot axes (X and Y)
enum ImAxisFlags_ {
    ImAxisFlags_GridLines  = 1 << 0, // grid lines will be displayed
    ImAxisFlags_TickMarks  = 1 << 1, // tick marks will be displayed for each grid line
    ImAxisFlags_TickLabels = 1 << 2, // text labels will be displayed for each grid line
    ImAxisFlags_Invert     = 1 << 3, // the axis will be inverted
    ImAxisFlags_LockMin    = 1 << 4, // the axis minimum value will be locked when panning/zooming
    ImAxisFlags_LockMax    = 1 << 5, // the axis maximum value will be locked when panning/zooming
    ImAxisFlags_Adaptive   = 1 << 6, // grid divisions will adapt to the current pixel size the axis
    ImAxisFlags_LogScale   = 1 << 7, // a logartithmic (base 10) axis scale will be used
    ImAxisFlags_Scientific = 1 << 8, // scientific notation will be used for tick labels if displayed (WIP)
    ImAxisFlags_Default    = ImAxisFlags_GridLines | ImAxisFlags_TickMarks | ImAxisFlags_TickLabels | ImAxisFlags_Adaptive
};

// Plot styling colors 
enum ImPlotCol_ {
    ImPlotCol_Line,          // plot item line/outline color (defaults to a rotating color set)
    ImPlotCol_Fill,          // plot item bar fill color (defaults to a rotating color set)
    ImPlotCol_MarkerOutline, // plot item marker outline color
    ImPlotCol_MarkerFill,    // plot item marker fill color
    ImPlotCol_FrameBg,       // plot frame background color (defaults to ImGuiCol_FrameBg)
    ImPlotCol_PlotBg,        // plot area background color (defaults to ImGuiCol_WindowBg)
    ImPlotCol_PlotBorder,    // plot area border color (defaults to ImGuiCol_Text)
    ImPlotCol_XAxis,         // x-axis grid/label color (defaults to ImGuiCol_Text)
    ImPlotCol_YAxis,         // x-axis grid/label color (defaults to ImGuiCol_Text)
    ImPlotCol_Selection,     // box-selection color (defaults to yellow)
    ImPlotCol_COUNT
};

enum ImPlotStyleVar_ {
    ImPlotStyleVar_LineWeight,   /// float, line weight in pixels
    ImPlotStyleVar_Marker,       /// int, marker specification
    ImPlotStyleVar_MarkerSize,   /// float, marker size in pixels (roughly the marker's "radius")
    ImPlotStyleVar_MarkerWeight, /// float, outline weight of markers
    ImPlotStyleVar_COUNT
};

// Marker specification
enum ImMarker_ {
    ImMarker_None        = 1 << 0,  // no marker
    ImMarker_Circle      = 1 << 1,  // a circle marker will be rendered at each point 
    ImMarker_Square      = 1 << 2,  // a square maker will be rendered at each point
    ImMarker_Diamond     = 1 << 3,  // a diamond marker will be rendered at each point
    ImMarker_Up          = 1 << 4,  // an upward-pointing triangle marker will up rendered at each point
    ImMarker_Down        = 1 << 5,  // an downward-pointing triangle marker will up rendered at each point
    ImMarker_Left        = 1 << 6,  // an leftward-pointing triangle marker will up rendered at each point
    ImMarker_Right       = 1 << 7,  // an rightward-pointing triangle marker will up rendered at each point
    ImMarker_Cross       = 1 << 8,  // a cross marker will be rendered at each point (not filled)
    ImMarker_Plus        = 1 << 9,  // a plus marker will be rendered at each point (not filled)
    ImMarker_Asterisk    = 1 << 10, // a asterisk marker will be rendered at each point (not filled)
};

// Plot style structure
struct ImPlotStyle {
    float        LineWeight;
    ImPlotMarker Marker;
    float        MarkerSize;
    float        MarkerWeight;
    ImVec4       Colors[ImPlotCol_COUNT];
    ImPlotStyle();
};

//-----------------------------------------------------------------------------
// Plot API
//-----------------------------------------------------------------------------

namespace ImGui {

// Starts a 2D plotting context. If this function returns true, EndPlot() must
// be called, e.g. "if (BeginPlot(...)) { ... EndPlot(); }"". #title_id must
// be unique. If you need to avoid ID collisions or don't want to display a
// title in the plot, use double hashes (e.g. "MyPlot##Hidden"). If #x_label
// and/or #y_label are provided, axes labels will be displayed. Flags are only
// set once during the first call to BeginPlot. 
bool BeginPlot(const char* title_id, 
               const char* x_label     = NULL, 
               const char* y_label     = NULL, 
               const ImVec2& size      = ImVec2(-1,-1), 
               ImPlotFlags flags       = ImPlotFlags_Default, 
               ImPlotAxisFlags x_flags = ImAxisFlags_Default, 
               ImPlotAxisFlags y_flags = ImAxisFlags_Default);
// Only call EndPlot() if BeginPlot() returns true! Typically called at the end
// of an if statement conditioned on BeginPlot().
void EndPlot();

/// Set the axes ranges of the next plot. Call right before BeginPlot().
void SetNextPlotAxes(float x_min, float x_max, float y_min, float y_max, ImGuiCond cond = ImGuiCond_Once);
/// Returns true if the plot in the current or most recent plot is hovered
bool IsPlotHovered();
/// Returns the mouse position in x,y coordinates of the current or most recent plot.
ImVec2 GetPlotMousePos();

//-----------------------------------------------------------------------------
// Plot Items
//-----------------------------------------------------------------------------

// Plots a standard 2D line and/or scatter plot 
void Plot(const char* label_id, const float* xs, const float* ys, int count, int offset = 0, int stride = sizeof(float));
void Plot(const char* label_id, ImVec2 (*getter)(void* data, int idx), void* data, int count, int offset = 0);
// Plots vertical bars
void PlotBar(const char* label_id, float width, const float* xs, const float* ys, int count, int offset = 0, int stride = sizeof(float));
void PlotBar(const char* label_id, float width, ImVec2 (*getter)(void* data, int idx), void* data, int count, int offset = 0);
// Plots horizontal bars (TODO)
void PlotBarH(const char* label_id, float height, const float* xs, const float* ys, int count, int offset = 0, int stride = sizeof(float));
void PlotBarH(const char* label_id, float height, ImVec2 (*getter)(void* data, int idx), void* data, int count, int offset = 0);
// Plots vertical stems (TODO)
void PlotStem(const char* label_id, const float* xs, const float* ys, int count, int offset = 0, int stride = sizeof(float));
// Plots error bars
void PlotErrorBars();
// Plots a text label at point x,y
void PlotLabel(const char* text, float x, float y, const ImVec2& pixel_offset = ImVec2(0,0));

//-----------------------------------------------------------------------------
// Plot Styling
//-----------------------------------------------------------------------------

// Special Color used to specific that a plot item color should set determined automatically.
#define IM_COL_AUTO ImVec4(0,0,0,-1)

// Provides access to plot style structure for permanant modifications to colors, sizes, etc.
ImPlotStyle& GetPlotStyle();

// Sets the color map to be used for plot items 
void SetPlotColorMap(const ImVec4* colors, int num_colors);
// Restores the default ImPlot color map 
void RestorePlotColorMap();

// Temporarily modify a plot color.
void PushPlotColor(ImPlotCol idx, ImU32 col);
// Temporarily modify a plot color.
void PushPlotColor(ImPlotCol idx, const ImVec4& col);
// Undo temporary color modification. 
void PopPlotColor(int count = 1);

// Temporarily modify a style variable of float type
void PushPlotStyleVar(ImPlotStyleVar idx, float val);
// Temporarily modify a style variable of int type
void PushPlotStyleVar(ImPlotStyleVar idx, int val);
// Undo temporary style modification.
void PopPlotStyleVar(int count = 1);

// Temporarily modify styling variables associated with plot line style. 
void PushLineStyle(float line_weight, const ImVec4& line_color);
// Undo temporary line style modification. 
void PopLineStyle(int count = 1);

// Temporarily modify styling variables associated with marker style. 
void PushMarkerStyle(ImPlotMarker marker, float marker_size, const ImVec4& marker_fill, const ImVec4& marker_outline);
// Undo temporary marker modification. 
void PopMarkerStyle(int count = 1);

// Temporarily modify styling variables associated with bar plot style. 
void PushBarStyle(float outline_weight, const ImVec4& bar_fill, const ImVec4& bar_outline);
// Undo temporary bar modification
void PopBarStyle(int count = 1);

//-----------------------------------------------------------------------------
// Demo
//-----------------------------------------------------------------------------

// Shows the ImPlot demo. Add implot_demo.cpp to your sources!
void ShowImPlotDemoWindow(bool* p_open = NULL);

}  // namespace ImGui