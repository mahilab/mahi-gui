#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <mahi/ImGui.hpp>
#include <imgui_internal.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <mahi/Print.hpp>

namespace ImGui {

void HoverTooltip(const char* tip, float delay) { 
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay) { 
        ImGui::SetTooltip("%s", tip); 
    }
}

void BeginDisabled(bool disabled, float alpha) {
    if (disabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
    }
}

/// Ends a disabled section
void EndDisabled(bool disabled) {
    if (disabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void EnableButton(const char* label, bool* enabled, const ImVec2& size) {
    bool dim = !*enabled;
    if (dim)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
    if (ImGui::Button(label, size))
        *enabled = !(*enabled);
    if (dim)
        ImGui::PopStyleVar();
}

///////////////////////////////////////////////////////////////////////////////
// PLOT
//////////////////////////////////////////////////////////////////////////////

namespace {

/// Utility function to that rounds x to powers of 2,5 and 10 for generating axis labels
inline double NiceNum(double x, bool round)
{
    double f;  /* fractional part of x */
    double nf; /* nice, rounded fraction */
    int expv = (int)floor(log10(x));
    f = x / std::pow(10, expv); /* between 1 and 10 */
    if (round)
        if (f < 1.5)
            nf = 1;
        else if (f < 3)
            nf = 2;
        else if (f < 7)
            nf = 5;
        else
            nf = 10;
    else if (f <= 1)
        nf = 1;
    else if (f <= 2)
        nf = 2;
    else if (f <= 5)
        nf = 5;
    else
        nf = 10;
    return nf * std::pow(10., expv);
}

inline void GetTicks(float tMin, float tMax, int nMajor, int nMinor, std::vector<double>& out) {
    out.clear();
    const double range = NiceNum(tMax - tMin, 0);
    const double interval = NiceNum(range / (nMajor - 1), 1);
    const double graphmin = floor(tMin / interval) * interval;
    const double graphmax = ceil(tMax / interval) * interval;
    const int nfrac = std::max((int)-floor(log10(interval)), 0);
    for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval)
    {
        out.push_back(major);
        for (int i = 1; i < nMinor; ++i)
            out.push_back(major + i * interval / nMinor);
    }
    out.resize(out.size() - nMinor + 1);
}

inline void RenderPlotItemLine(const PlotItem& item, const PlotInterface& plot, const ImRect& bb, ImDrawList& DrawList) {
    static std::vector<ImVec2> pointsPx(10000); // up front allocation
    pointsPx.resize(item.data.size());
    // transform data
    for (int i = 0; i < item.data.size(); ++i) {
        pointsPx[i].x = Remap(item.data[i].x, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        pointsPx[i].y = Remap(item.data[i].y, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
    }    
    DrawList.AddPolyline(&pointsPx[0], (int)pointsPx.size(), GetColorU32(item.color), false, item.size);    
}

inline void RenderPlotItemScatter(const PlotItem& item, const PlotInterface& plot, const ImRect& bb, ImDrawList& DrawList) {
    ImU32 col = GetColorU32(item.color);
    for (int i = 0; i < item.data.size(); ++i) {
        ImVec2 c;
        c.x = Remap(item.data[i].x, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        c.y = Remap(item.data[i].y, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
        DrawList.AddCircleFilled(c, item.size, col, 10);
    }    
}

inline void RenderPlotItemXBar(const PlotItem& item, const PlotInterface& plot, const ImRect& bb, ImDrawList& DrawList) {
    ImU32 col = GetColorU32(item.color);
    for (int i = 0; i < item.data.size(); ++i) {
        if (item.data[i].y == 0)
            continue;
        float y1 = Remap(item.data[i].y, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
        float y2 = Remap(0, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
        float l = Remap(item.data[i].x - item.size * 0.5f, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        float r = Remap(item.data[i].x + item.size * 0.5f, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        ImVec2 Min = {l, ImMin(y1,y2)};
        ImVec2 Max = {r, ImMax(y1,y2)};
        DrawList.AddRectFilled(Min, Max, col);
    }
}

inline void RenderPlotItemYBar(const PlotItem& item, const PlotInterface& plot, const ImRect& bb, ImDrawList& DrawList) {
    ImU32 col = GetColorU32(item.color);
    for (int i = 0; i < item.data.size(); ++i) {
        if (item.data[i].x == 0)
            continue;
        float x1 = Remap(item.data[i].x, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        float x2 = Remap(0, plot.xAxis.minimum, plot.xAxis.maximum, bb.Min.x, bb.Max.x);
        float t = Remap(item.data[i].y + item.size * 0.5f, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
        float b = Remap(item.data[i].y - item.size * 0.5f, plot.yAxis.minimum, plot.yAxis.maximum, bb.Max.y, bb.Min.y);
        ImVec2 Min = {ImMin(x1,x2), t};
        ImVec2 Max = {ImMax(x1,x2), b};
        DrawList.AddRectFilled(Min, Max, col);
    }
}

} // private namespace

PlotItem::PlotItem() : 
    show(true), type(PlotItem::Line), data(), color(1,1,1,1), size(1) {
}

PlotAxis::PlotAxis() :
    show(true), minimum(0), maximum(1), divisions(4), subDivisions(10),
    color(ImGui::GetStyle().Colors[ImGuiCol_Border] * ImVec4(1,1,1,0.25f)),
    zoomRate(0.1f)
{

}

PlotInterface::PlotInterface() {
    frameColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
    backgroundColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    borderColor = ImGui::GetStyle().Colors[ImGuiCol_Border];
}

bool Plot(const char* label_id, PlotInterface& plot, const ImVec2& size) {
    bool changed = false;
    // ImGui front matter
    ImGuiWindow* Window = GetCurrentWindow();
    if (Window->SkipItems)
        return false;
    ImGuiContext& G = *GImGui;
    const ImGuiStyle& Style = G.Style;
    const ImGuiIO& IO = GetIO();
    ImDrawList& DrawList = *Window->DrawList; 

    const ImGuiID id = Window->GetID(label_id);

    // colors
    const ImU32 color_frame = GetColorU32(plot.frameColor);
    const ImU32 color_bg = GetColorU32(plot.backgroundColor);
    const ImU32 color_border = GetColorU32(plot.borderColor);
    const ImU32 color_x1 = GetColorU32(plot.xAxis.color);
    const ImU32 color_x2 = GetColorU32(plot.xAxis.color * ImVec4(1,1,1,0.25f));
    const ImU32 color_y1 = GetColorU32(plot.yAxis.color);
    const ImU32 color_y2 = GetColorU32(plot.yAxis.color * ImVec4(1,1,1,0.25f));

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    const ImRect frame_bb(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(frame_bb);
    if (!ItemAdd(frame_bb, 0, &frame_bb))
        return false;
    const bool frame_hovered = ItemHoverable(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, color_frame, true, Style.FrameRounding);

    // grid axis
    const ImRect grid_bb(frame_bb.Min + Style.WindowPadding, frame_bb.Max - Style.WindowPadding);
    const bool grid_hovered = grid_bb.Contains(IO.MousePos);
    // grid bg
    DrawList.AddRectFilled(grid_bb.Min, grid_bb.Max, color_bg);

    // end drag
    if (IO.MouseReleased[0] || !IO.MouseDown[0])
        plot._dragging = false;  
    // drag
    if (plot._dragging) {
        float delX = IO.MouseDelta.x * (plot.xAxis.maximum - plot.xAxis.minimum) / (grid_bb.Max.x - grid_bb.Min.x);
        float delY = IO.MouseDelta.y * (plot.yAxis.maximum - plot.yAxis.minimum) / (grid_bb.Max.y - grid_bb.Min.y);
        plot.xAxis.minimum -= delX;
        plot.xAxis.maximum -= delX;
        plot.yAxis.minimum += delY;
        plot.yAxis.maximum += delY;
    }
    // start drag
    if (frame_hovered && grid_hovered && IO.MouseClicked[0]) 
        plot._dragging = true;

    // scroll zoom
    if (frame_hovered && grid_hovered) {
        float xRange = plot.xAxis.maximum - plot.xAxis.minimum;
        float yRange = plot.yAxis.maximum - plot.yAxis.minimum;
        if (IO.MouseWheel < 0) {
            plot.xAxis.minimum -= plot.xAxis.zoomRate * xRange;
            plot.xAxis.maximum += plot.xAxis.zoomRate * xRange;
            plot.yAxis.minimum -= plot.yAxis.zoomRate * yRange;
            plot.yAxis.maximum += plot.yAxis.zoomRate * yRange;
        }
        if (IO.MouseWheel > 0) {
            plot.xAxis.minimum += plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
            plot.xAxis.maximum -= plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
            plot.yAxis.minimum += plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
            plot.yAxis.maximum -= plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
        }
    }

    // render axes
    static std::vector<double> ticks(100);
    DrawList.PushClipRect(grid_bb.Min, grid_bb.Max);

    // x-axis
    if (plot.xAxis.maximum <= plot.xAxis.minimum)
        plot.xAxis.maximum = plot.xAxis.minimum + std::numeric_limits<float>::epsilon();
    if (plot.xAxis.show && plot.xAxis.divisions > 1) {
        GetTicks(plot.xAxis.minimum, plot.xAxis.maximum, plot.xAxis.divisions, plot.xAxis.subDivisions, ticks);
        for (int i = 0; i < ticks.size(); ++i) {
            float x = Remap((float)ticks[i], plot.xAxis.minimum, plot.xAxis.maximum, grid_bb.Min.x, grid_bb.Max.x);
            x = std::round(x);
            if (x >= grid_bb.Min.x && x <= grid_bb.Max.x) {
                bool major = plot.xAxis.subDivisions > 0 ? i % plot.xAxis.subDivisions == 0 : true;
                ImVec2 p1(x, grid_bb.Min.y);
                ImVec2 p2(x, grid_bb.Max.y);
                DrawList.AddLine(p1,p2, major ? color_x1 : color_x2,1);
            }
        }
    }
    // y-axis
    if (plot.yAxis.maximum <= plot.yAxis.minimum)
        plot.yAxis.maximum = plot.yAxis.minimum + std::numeric_limits<float>::epsilon();
    if (plot.yAxis.show && plot.yAxis.divisions > 1) {
        GetTicks(plot.yAxis.minimum, plot.yAxis.maximum, plot.yAxis.divisions, plot.yAxis.subDivisions, ticks);
        for (int i = 0; i < ticks.size(); ++i) {
            float y = Remap((float)ticks[i], plot.yAxis.minimum, plot.yAxis.maximum, grid_bb.Max.y, grid_bb.Min.y);
            y = std::round(y);
            if (y >= grid_bb.Min.y && y <= grid_bb.Max.y) {
                bool major = plot.yAxis.subDivisions > 0 ? i % plot.yAxis.subDivisions == 0 : true;
                ImVec2 p1(grid_bb.Min.x, y);
                ImVec2 p2(grid_bb.Max.x, y);
                DrawList.AddLine(p1,p2, major ? color_y1 : color_y2,1);
            }
        }
    }
    
    // render 
    for (auto& item : plot.items) {
        if (item.show) {
            if (item.type == PlotItem::Line)
                RenderPlotItemLine(item, plot, grid_bb, DrawList);
            else if (item.type == PlotItem::Scatter)
                RenderPlotItemScatter(item, plot, grid_bb, DrawList);
            else if (item.type == PlotItem::XBar)
                RenderPlotItemXBar(item, plot, grid_bb, DrawList);
            else if (item.type == PlotItem::YBar)
                RenderPlotItemYBar(item, plot, grid_bb, DrawList);
        }
    }   

    DrawList.PopClipRect();   
    DrawList.AddRect(grid_bb.Min, grid_bb.Max, color_border);

    return true;
}

}