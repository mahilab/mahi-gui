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
typedef int ImPlotSpec;

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
enum ImPlotAxisFlags_ {
    ImPlotAxisFlags_GridLines  = 1 << 0, // grid lines will be displayed
    ImPlotAxisFlags_TickMarks  = 1 << 1, // tick marks will be displayed for each grid line
    ImPlotAxisFlags_TickLabels = 1 << 2, // text labels will be displayed for each grid line
    ImPlotAxisFlags_Invert     = 1 << 3, // the axis will be inverted
    ImPlotAxisFlags_LockMin    = 1 << 4, // the axis minimum value will be locked when panning/zooming
    ImPlotAxisFlags_LockMax    = 1 << 5, // the axis maximum value will be locked when panning/zooming
    ImPlotAxisFlags_Adaptive   = 1 << 6, // grid divisions will adapt to the current pixel size the axis
    ImPlotAxisFlags_LogScale   = 1 << 7, // a logartithmic (base 10) axis scale will be used
    ImPlotAxisFlags_Scientific = 1 << 8, // scientific notation will be used for tick labels if displayed
    ImPlotAxisFlags_Default    = ImPlotAxisFlags_GridLines | ImPlotAxisFlags_TickMarks | ImPlotAxisFlags_TickLabels | ImPlotAxisFlags_Adaptive
};

// Plot styling colors 
enum ImPlotCol_ {
    ImPlotCol_Line,        // plot item line/outline color (defaults to a rotating color set)
    ImPlotCol_Fill,        // plot item marker/bar fill color (defaults to a rotating color set)
    ImPlotCol_FrameBg,     // plot frame background color (defaults to ImGuiCol_FrameBg)
    ImPlotCol_PlotBg,      // plot area background color (defaults to ImGuiCol_WindowBg)
    ImPlotCol_PlotBorder,  // plot area border color (defaults to ImGuiCol_Text)
    ImPlotCol_XAxis,       // x-axis grid/label color (defaults to ImGuiCol_Text)
    ImPlotCol_YAxis,       // x-axis grid/label color (defaults to ImGuiCol_Text)
    ImPlotCol_Selection,   // box-selection color (defaults to yellow)
    ImPlotCol_COUNT
};

// Plot item appearance specification
enum ImPlotSpec_ {
    ImPlotSpec_SolidLine   = 1 << 0,  // solid lines will be rendered between consecutive points
    ImPlotSpec_Circle      = 1 << 1,  // a circle marker will be rendered at each point 
    ImPlotSpec_Square      = 1 << 2,  // a square maker will be rendered at each point
    ImPlotSpec_Diamond     = 1 << 3,  // a diamond marker will be rendered at each point
    ImPlotSpec_Up          = 1 << 4,  // an upward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Down        = 1 << 5,  // an downward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Left        = 1 << 6,  // an leftward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Right       = 1 << 7,  // an rightward-pointing triangle marker will up rendered at each point
    ImPlotSpec_Cross       = 1 << 8,  // a cross marker will be rendered at each point (not filled)
    ImPlotSpec_Plus        = 1 << 9,  // a plus marker will be rendered at each point (not filled)
    ImPlotSpec_Asterisk    = 1 << 10, // a asterisk marker will be rendered at each point (not filled)
};

// Plot style structure
struct ImPlotStyle {
    ImPlotSpec Spec;
    float MarkerSize;
    float LineWeight;
    ImVec4 Colors[ImPlotCol_COUNT];
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
               ImPlotAxisFlags x_flags = ImPlotAxisFlags_Default, 
               ImPlotAxisFlags y_flags = ImPlotAxisFlags_Default);
// Only call EndPlot() if BeginPlot() returns true! Typically called at the end
// of an if statement conditioned on BeginPlot().
void EndPlot();

/// Set the axes ranges of the next plot. Call right before BeginPlot().
void SetNextPlotAxes(float xmin, float xmax, float ymin, float ymax, ImGuiCond cond = ImGuiCond_Once);
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

// Provides access to plot style structure (colors, sizes, etc.)
ImPlotStyle& GetPlotStyle();
// TODO
void SetPlotColorMap(const ImVec4* colors, int num_colors);
// Sets the plot line and marker specification to be used on subsequent plot items.
void SetPlotSpec(ImPlotSpec spec);
// Sets plot line styling variables to be used on subsequent plot items.
void SetPlotLineStyle(float line_weight, const ImVec4& line_color = IM_COL_AUTO);
// Sets plot marker styling variables to be used on subsequent plot items.
void SetPlotMarkerStyle(float marker_size, const ImVec4& fill_color = ImVec4(0,0,0,0));
// Sets the complete style to be used on subsequent plot items. The special color {0,0,0,-1} generates automatic colors.
void SetPlotStyle(ImPlotSpec spec, float line_weight, const ImVec4& line_color, float marker_size, const ImVec4& fill_color);
// TODO
void PushPlotColor(ImPlotCol idx, ImU32 col);
// TODO
void PushPlotColor(ImPlotCol idx, const ImVec4& col);
// TODO
void PopPlotColor(int count);

}  // namespace ImGui