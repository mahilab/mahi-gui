#pragma once
#include <imgui.h>
#include <imgui_stdlib.h>
#include <vector>

namespace ImGui {

///////////////////////////////////////////////////////////////////////////////
// PLOT STRUCTURES
///////////////////////////////////////////////////////////////////////////////

/// An item to be plotted (e.g line, scatter, or bar plot).
struct PlotItem {
    /// Supported plot types.
    enum Type {
        Line,    // Data plotted as a olid line.
        Scatter, // Data plotted as discontinuous circles.
        XBar,    // Data plotted as centered bars plot emanating from X-axis.
        YBar     // Data plotted as centered bars plot emanating from Y axis.
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
    /// The size of the item (Line = pixel width, Scatter = pixel diameter, XBar/YBar = bar width in plot units).
    float size;
    /// The label to be displayed in any legends.
    std::string label; 
    /// For internal usage only.
    int _begin;
};

/// A plot axis (X or Y)
struct PlotAxis {
    /// Default
    PlotAxis();
    /// Should the axis grid lines be shown?
    bool showGrid;
    /// Should the axis tick marks be shown?
    bool showTickMarks;
    /// Should the axis tick labels be shown?
    bool showTickLabels;
    /// Axis minimum value.
    float minimum;
    /// Axis maximum value.
    float maximum;
    /// Number of grid divisions.
    int divisions;
    /// Number of grid subdivisions.
    int subDivisions;
    /// Axis color (affects grid lines, ticks, and labels)
    ImVec4 color;
    /// The percentage of the axis range that each mouse scroll adds/subracts from view (default=0.1).
    float zoomRate;
    /// Is the minimum locked from zooming?
    bool lockMin;
    /// Is the maximum locked from zooming?
    bool lockMax;
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
    PlotAxis xAxis;
    /// The Y-Axis
    PlotAxis yAxis;
    /// Should crosshairs be shown when the plot is hovered?
    bool showCrosshairs;
    /// Should the x-y position of the hovered mouse be shown?
    bool showMousePos;
    /// Should the plot items legend be shown?
    bool showLegend;
    /// Is right-click selection allowed?
    bool enableSelection;
    /// Should middle-click open the controls pop-up?
    bool enableControls;
    /// The color of the plot frame (i.e. outer background)
    ImVec4 frameColor;
    /// The color of the grid background (i.e. inner background)
    ImVec4 backgroundColor;
    /// The color of the grid border
    ImVec4 borderColor;  
    /// The color of the selection box tool
    ImVec4 selectionColor;
    /// The plot title (default="", i.e. no title displayed)
    std::string title;
    /// For internal use only.
    bool _dragging;
    bool _selecting;
    ImVec2 _selectStart;
};

///////////////////////////////////////////////////////////////////////////////
// RENDERER
///////////////////////////////////////////////////////////////////////////////

/// Renders a Plot and PlotItems
void Plot(const char* label_id, PlotInterface* plot, PlotItem* items, int nItems, const ImVec2& size = {-1,-1});
/// Renders a Plot and PlotItems (STL version)
void Plot(const char* label_id, PlotInterface& plot, std::vector<PlotItem>& items, const ImVec2& size = {-1,-1});

///////////////////////////////////////////////////////////////////////////////
// REAL-TIME UTILS
///////////////////////////////////////////////////////////////////////////////

/// Adds a point to an item data set, clearing it every #span seconds
inline void PlotItemRollPoint(PlotItem& item, float x, float y, float span = 10) {
    float xmod = fmod(x, span);
    if (!item.data.empty() && xmod < item.data.back().x)
        item.data.clear();
    item.data.push_back(ImVec2(xmod, y));
}

/// Pushes a point into an item data set as if it were a circular buffer of #maxPoints size
inline void PlotItemBufferPoint(PlotItem& item, float x, float y, int maxPoints) {
    if (item.data.size() < maxPoints) 
        item.data.push_back(ImVec2(x,y));
    else {
        item.data[item._begin] = ImVec2(x,y);
        item._begin++;
        if (item._begin == maxPoints)
            item._begin = 0;
    }
}

/// Call before rendering a plot to scroll the axis in time, displaying #history seconds
inline void PlotAxisScroll(PlotAxis& axis, float currentTime, float history) {
    axis.maximum = currentTime;
    axis.minimum = currentTime - history;
}

} // namespce ImGui