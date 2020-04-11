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

namespace ImGui {

///////////////////////////////////////////////////////////////////////////////
// STRUCTS
///////////////////////////////////////////////////////////////////////////////

/// An item to be plotted (e.g line, scatter, or bar plot).
struct PlotItem {
    /// Supported plot types.
    enum Type {
        Line,     // Data plotted as a solid line.
        Scatter,  // Data plotted as discontinuous circles.
        XBar,     // Data plotted as centered bars plot emanating from X-axis.
        YBar      // Data plotted as centered bars plot emanating from Y axis.
    };
    /// Default constructor.
    PlotItem();
    /// Should this item be shown?
    bool show;
    /// The type of plot this is.
    Type type;
    /// XY data points.
    std::vector<ImVec2> data;
    /// The item color.
    ImVec4 color;
    /// The size of the item (Line = pixel width, Scatter = pixel diameter, XBar/YBar = bar width in
    /// plot units).
    float size;
    /// The label to be displayed in any legends.
    std::string label;
    /// The "first" data element index, can be used for ring-buffering Line and Scatter types
    int data_begin;
};

/// A plot axis (X or Y)
struct PlotAxis {
    /// Default
    PlotAxis();
    /// Should the axis grid lines be shown?
    bool show_grid;
    /// Should the axis tick marks be shown?
    bool show_tick_marks;
    /// Should the axis tick labels be shown?
    bool show_tick_labels;
    /// Axis minimum value.
    float minimum;
    /// Axis maximum value.
    float maximum;
    /// Number of grid divisions.
    int divisions;
    /// Number of grid subdivisions.
    int subdivisions;
    /// Adaptive divisions based on pixels
    bool adaptive;
    /// Axis color (affects grid lines, ticks, and labels)
    ImVec4 color;
    /// The percentage of the axis range that each mouse scroll adds/subracts from view
    /// (default=0.1).
    float zoom_rate;
    /// Is the minimum locked from zooming?
    bool lock_min;
    /// Is the maximum locked from zooming?
    bool lock_max;
    /// Should the axis be flipped?
    bool flip;
    /// The label of the axis (default="", i.e. no label displayed).
    std::string label;
};

/// A 2D plot interface.
struct PlotInterface {
    /// Default constructor.
    PlotInterface();
    /// The X-Axis
    PlotAxis x_axis;
    /// The Y-Axis
    PlotAxis y_axis;
    /// Should crosshairs be shown when the plot is hovered?
    bool show_crosshairs;
    /// Should the x-y position of the hovered mouse be shown?
    bool show_mouse_pos;
    /// Should the plot items legend be shown?
    bool show_legend;
    /// Is right-click selection allowed?
    bool enable_selection;
    /// Should middle-click open the controls pop-up?
    bool enable_controls;
    /// The color of the plot frame (i.e. outer background)
    ImVec4 frame_color;
    /// The color of the grid background (i.e. inner background)
    ImVec4 background_color;
    /// The color of the grid border
    ImVec4 border_color;
    /// The color of the selection box tool
    ImVec4 selection_color;
    /// The plot title (default="", i.e. no title displayed)
    std::string title;
    /// [Internal]
    bool   _dragging_x;
    bool   _dragging_y;
    bool   _selecting;
    ImVec2 _select_start;
};

///////////////////////////////////////////////////////////////////////////////
// RENDERER
///////////////////////////////////////////////////////////////////////////////

/// Renders a Plot and PlotItems
bool Plot(const char* label_id, PlotInterface* plot, PlotItem* items, int num_items,
          const ImVec2& size = {-1, -1});
/// Renders a Plot and PlotItems (STL version)
bool Plot(const char* label_id, PlotInterface& plot, std::vector<PlotItem>& items,
          const ImVec2& size = {-1, -1});

///////////////////////////////////////////////////////////////////////////////
// REAL-TIME UTILS
///////////////////////////////////////////////////////////////////////////////

/// Adds a point to an item data set, clearing it every #span seconds
void PlotItemRollPoint(PlotItem& item, double x, double y, double span = 10);
/// Pushes a point into an item data set as if it were a circular buffer of #max_points size
void PlotItemBufferPoint(PlotItem& item, double x, double y, int max_points);
/// Call before rendering a plot to scroll the axis in time, displaying #history seconds
void PlotAxisScroll(PlotAxis& axis, double current_time, double history);

////////////////////////////////////////////////////////////////////////////////
// MISC UTIL
////////////////////////////////////////////////////////////////////////////////

/// Returns true if the plot in the last call to ImGui::Plot is hovered
bool IsPlotHovered();
/// Returns the mouse position in x,y coordinates of the last ImGui::Plot
ImVec2 GetPlotMousePos();

}  // namespace ImGui