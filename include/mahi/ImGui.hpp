#pragma once
#include <imgui.h>
#include <vector>

// mahi-gui custom ImGui
namespace ImGui {

/// Shows a tooltip when an item is hovered for longer than delay
void HoverTooltip(const char* tip, float delay = 1.0f);

/// Begins a disabled section
void BeginDisabled(bool disabled = true, float alpha = 0.25f);

/// Ends a disabled section
void EndDisabled(bool disabled = true);

/// A button that acts like a toggle, dimming itself when disabled
void EnableButton(const char* label, bool* enabled, const ImVec2& size = ImVec2(0,0));

/// Linearly remaps float x from [x0 x1] to [y0 y1]
inline float Remap(float x, float x0, float x1, float y0, float y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

/// Linearly remaps ImVec2 x from [x0 x1] to [y0 y1]
inline ImVec2 Remap(const ImVec2& x, const ImVec2& x0, const ImVec2& x1, const ImVec2& y0, const ImVec2& y1) {
    ImVec2 out;
    out.x = Remap(x.x, x0.x, x1.x, y0.x, y1.x);
    out.y = Remap(x.y, x0.y, x1.y, y0.y, y1.y);
    return out;
}

///////////////////////////////////////////////////////////////////////////////
// PLOT
///////////////////////////////////////////////////////////////////////////////

struct PlotItem {
    enum Type {
        Line,
        Scatter,
        XBar,
        YBar
    };
    PlotItem();
    bool show;
    Type type;
    std::vector<ImVec2> data;
    ImVec4 color;
    float size; 
};

struct PlotAxis {
    PlotAxis();
    bool show;
    float minimum;
    float maximum;
    int divisions;
    int subDivisions;
    ImVec4 color;
    float zoomRate;
};

struct PlotInterface {
    PlotInterface();
    PlotAxis xAxis;
    PlotAxis yAxis;
    ImVec4 frameColor;
    ImVec4 backgroundColor;
    ImVec4 borderColor;  
    std::vector<PlotItem> items;
    // internal
    bool _dragging;
};

bool Plot(const char* label_id, PlotInterface& plot, const ImVec2& size = {-1,-1});

} // namespace ImGui