#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <Mahi/Gui/imgui_plot.hpp>
#include <Mahi/Gui/imgui_custom.hpp>
#include <imgui_internal.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Gui/Color.hpp>
#include <sstream>

namespace ImGui {

// private namespace
namespace
{

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

struct Tick
{
    double plot;
    float pixels;
    bool major;
    std::string txt;
    ImVec2 size;
};

inline void GetTicks(float tMin, float tMax, int nMajor, int nMinor, std::vector<Tick> &out)
{
    out.clear();
    const double range = NiceNum(tMax - tMin, 0);
    const double interval = NiceNum(range / (nMajor - 1), 1);
    const double graphmin = floor(tMin / interval) * interval;
    const double graphmax = ceil(tMax / interval) * interval;
    const int nfrac = std::max((int)-floor(log10(interval)), 0);
    for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval)
    {
        if (major >= tMin && major <= tMax)
            out.push_back({major, 0, true});
        for (int i = 1; i < nMinor; ++i)
        {
            double minor = major + i * interval / nMinor;
            if (minor >= tMin && minor <= tMax)
                out.push_back({minor, 0, false});
        }
    }
}

inline void LabelTicks(std::vector<Tick> &ticks)
{
    for (auto &tk : ticks)
    {
        std::stringstream ss;
        ss << tk.plot;
        tk.txt = ss.str();
        tk.size = CalcTextSize(tk.txt.c_str());
    }
}

inline void TransformTicks(std::vector<Tick> &ticks, float tMin, float tMax, float pMin, float pMax)
{
    const float m = (pMax - pMin) / (tMax - tMin);
    for (auto &tk : ticks)
        tk.pixels = (pMin + m * (tk.plot - tMin)); // IM_ROUND used previoiusly, but causes distortion in moving plot
}

inline void RenderPlotItemLine(const PlotItem& item, const PlotInterface& plot, const ImRect& pix, ImDrawList& DrawList) {
    if (item.data.size() < 2)
        return;
    static std::vector<ImVec2> pointsPx(10000); // up front allocation
    pointsPx.resize(item.data.size());
    const float mx = (pix.Max.x - pix.Min.x) / (plot.xAxis.maximum - plot.xAxis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.yAxis.maximum - plot.yAxis.minimum);
    // transform data
    for (int i = 0; i < item.data.size(); ++i)
    {
        pointsPx[i].x = pix.Min.x + mx * (item.data[i].x - plot.xAxis.minimum);
        pointsPx[i].y = pix.Min.y + my * (item.data[i].y - plot.yAxis.minimum);
    }
    auto color = GetColorU32(item.color);
    int segments = (int)item.data.size() - 1;
    int i = item._begin;
    for (int s = 0; s < segments; ++s)
    {
        int j = i + 1;
        if (j == item.data.size())
            j = 0;
        DrawList.AddLine(pointsPx[i], pointsPx[j], color, item.size);
        i = j;
    }
}

inline void RenderPlotItemScatter(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.xAxis.maximum - plot.xAxis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.yAxis.maximum - plot.yAxis.minimum);
    for (int i = 0; i < item.data.size(); ++i)
    {
        ImVec2 c;
        c.x = pix.Min.x + mx * (item.data[i].x - plot.xAxis.minimum);
        c.y = pix.Min.y + my * (item.data[i].y - plot.yAxis.minimum);
        DrawList.AddCircleFilled(c, item.size, col, 10);
    }
}

inline void RenderPlotItemXBar(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.xAxis.maximum - plot.xAxis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.yAxis.maximum - plot.yAxis.minimum);
    const float halfSize = 0.5f * item.size;
    for (int i = 0; i < item.data.size(); ++i)
    {
        if (item.data[i].y == 0)
            continue;
        float y1 = pix.Min.y + my * (item.data[i].y - plot.yAxis.minimum);
        float y2 = pix.Min.y + my * (-plot.yAxis.minimum);
        float l = pix.Min.x + mx * (item.data[i].x - halfSize - plot.xAxis.minimum);
        float r = pix.Min.x + mx * (item.data[i].x + halfSize - plot.xAxis.minimum);
        DrawList.AddRectFilled({l, ImMin(y1, y2)}, {r, ImMax(y1, y2)}, col);
    }
}

inline void RenderPlotItemYBar(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.xAxis.maximum - plot.xAxis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.yAxis.maximum - plot.yAxis.minimum);
    const float halfSize = 0.5f * item.size;
    for (int i = 0; i < item.data.size(); ++i)
    {
        if (item.data[i].x == 0)
            continue;
        float x1 = pix.Min.x + mx * (item.data[i].x - plot.xAxis.minimum);
        float x2 = pix.Min.x + mx * (0 - plot.xAxis.minimum);
        float t = pix.Min.y + my * (item.data[i].y + halfSize - plot.yAxis.minimum);
        float b = pix.Min.y + my * (item.data[i].y - halfSize - plot.yAxis.minimum);
        DrawList.AddRectFilled({ImMin(x1, x2), t}, {ImMax(x1, x2), b}, col);
    }
}

} // namespace

PlotItem::PlotItem() : show(true), type(PlotItem::Line), data(), size(1), _begin(0)
{
    using namespace mahi::gui;
    static std::vector<ImVec4> colors = {Blues::DeepSkyBlue, Reds::Red, Greens::Chartreuse, Yellows::Yellow, Cyans::Cyan, Oranges::Orange, Purples::Magenta, Purples::BlueViolet, Cyans::LightCyan, Browns::Tan};
    static int nextColor = 0;
    static int itemNumber = 0;
    color = colors[nextColor];
    nextColor = (nextColor + 1) % colors.size();
    label = "item" + std::to_string(itemNumber++);
}

PlotAxis::PlotAxis() : showGrid(true), showTickMarks(true), showTickLabels(true), minimum(0), maximum(1), divisions(4), subDivisions(5),
                       color(0,0,0,-1),
                       zoomRate(0.1f), lockMin(false), lockMax(false), flip(false), label("")
{
}

PlotInterface::PlotInterface() : showCrosshairs(false), showMousePos(true), showLegend(true), enableSelection(true), enableControls(true), title(""),
                                 _dragging(false), _selecting(false)
{
    frameColor = ImVec4(0,0,0,-1);
    backgroundColor = ImVec4(0,0,0,-1);
    borderColor = ImVec4(0,0,0,-1);
    selectionColor = {.118f, .565f, 1, 0.25};
}

void Plot(const char *label_id, PlotInterface &plot, std::vector<PlotItem> &items, const ImVec2 &size)
{
    Plot(label_id, &plot, &items[0], items.size(), size);
}

void Plot(const char *label_id, PlotInterface *plot_ptr, PlotItem *items, int nItems, const ImVec2 &size)
{
    PlotInterface &plot = *plot_ptr;
    // ImGui front matter
    ImGuiWindow *Window = GetCurrentWindow();
    if (Window->SkipItems)
        return;
    ImGuiContext &G = *GImGui;
    const ImGuiStyle &Style = G.Style;
    const ImGuiIO &IO = GetIO();
    ImDrawList &DrawList = *Window->DrawList;
    const ImGuiID id = Window->GetID(label_id);

    // get colors
    const ImU32 color_frame = plot.frameColor.w == -1 ? GetColorU32(ImGuiCol_FrameBg) : GetColorU32(plot.frameColor);
    const ImU32 color_bg = plot.backgroundColor.w == -1 ? GetColorU32(ImGuiCol_WindowBg) : GetColorU32(plot.backgroundColor);
    const ImU32 color_border = plot.borderColor.w == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(plot.borderColor);

    const ImVec4 color_xAxis = plot.xAxis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.xAxis.color;
    const ImU32 color_x1 = GetColorU32(color_xAxis);
    const ImU32 color_x2 = GetColorU32(color_xAxis * ImVec4(1, 1, 1, 0.25f));
    const ImU32 color_xtxt = GetColorU32({color_xAxis.x, color_xAxis.y, color_xAxis.z, 1});

    const ImVec4 color_yAxis = plot.yAxis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.yAxis.color;
    const ImU32 color_y1 = GetColorU32(color_yAxis);
    const ImU32 color_y2 = GetColorU32(color_yAxis * ImVec4(1, 1, 1, 0.25f));
    const ImU32 color_ytxt = GetColorU32({color_yAxis.x, color_yAxis.y, color_yAxis.z, 1});

    const ImU32 color_txt = GetColorU32(ImGuiCol_Text);
    const ImU32 color_txt_dis = GetColorU32(ImGuiCol_TextDisabled);
    const ImU32 color_slctBg = GetColorU32(plot.selectionColor);
    const ImU32 color_slctBd = GetColorU32({plot.selectionColor.x, plot.selectionColor.y, plot.selectionColor.z, 1});

    // constrain axes
    if (plot.xAxis.maximum <= plot.xAxis.minimum)
        plot.xAxis.maximum = plot.xAxis.minimum + std::numeric_limits<float>::epsilon();
    if (plot.yAxis.maximum <= plot.yAxis.minimum)
        plot.yAxis.maximum = plot.yAxis.minimum + std::numeric_limits<float>::epsilon();

    // get ticks
    static std::vector<Tick> xTicks(100), yTicks(100);
    const bool renderX = (plot.xAxis.showGrid || plot.xAxis.showTickMarks || plot.xAxis.showTickLabels) && plot.xAxis.divisions > 1;
    const bool renderY = (plot.yAxis.showGrid || plot.yAxis.showTickMarks || plot.yAxis.showTickLabels) && plot.yAxis.divisions > 1;

    if (renderX)
        GetTicks(plot.xAxis.minimum, plot.xAxis.maximum, plot.xAxis.divisions, plot.xAxis.subDivisions, xTicks);
    if (renderY)
        GetTicks(plot.yAxis.minimum, plot.yAxis.maximum, plot.yAxis.divisions, plot.yAxis.subDivisions, yTicks);

    // label ticks
    if (plot.xAxis.showTickLabels)
        LabelTicks(xTicks);
    if (plot.yAxis.showTickLabels)
        LabelTicks(yTicks);

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    const ImRect frame_bb(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(frame_bb);
    if (!ItemAdd(frame_bb, 0, &frame_bb))
        return;
    const bool frame_hovered = ItemHoverable(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, color_frame, true, Style.FrameRounding);

    // canvas bb
    const ImRect canvas_bb(frame_bb.Min + Style.WindowPadding, frame_bb.Max - Style.WindowPadding);

    // get max y-tick width
    float maxLabelWidth = 0;
    if (plot.yAxis.showTickLabels)
    {
        for (auto &yt : yTicks)
            maxLabelWidth = yt.size.x > maxLabelWidth ? yt.size.x : maxLabelWidth;
    }

    // grid bb
    const float textOffset = 5;
    const float textHeight = GetTextLineHeight();
    const float tPadding = plot.title != "" ? textHeight + textOffset : 0;
    const float bPadding = (plot.xAxis.showTickLabels ? textHeight + textOffset : 0) + (plot.xAxis.label != "" ? textHeight + textOffset : 0);
    const float lPadding = (plot.yAxis.showTickLabels ? maxLabelWidth + textOffset : 0) + (plot.yAxis.label != "" ? textHeight + textOffset : 0);
    const ImRect grid_bb(canvas_bb.Min + ImVec2(lPadding, tPadding), canvas_bb.Max - ImVec2(0, bPadding));
    const bool grid_hovered = grid_bb.Contains(IO.MousePos);

    // axis region bb
    const ImRect xAxisRegion_bb(grid_bb.Min + ImVec2(10,0), {grid_bb.Max.x, frame_bb.Max.y});
    const bool xAxisRegion_hovered = xAxisRegion_bb.Contains(IO.MousePos);
    const ImRect yAxisRegion_bb({frame_bb.Min.x, grid_bb.Min.y}, grid_bb.Max - ImVec2(0,10));
    const bool yAxisRegion_hovered = yAxisRegion_bb.Contains(IO.MousePos);

    // legend bb
    const ImVec2 legend_offset(10, 10);
    const ImVec2 legend_padding(5, 5);
    const float legend_icon_size = textHeight;

    ImRect legend_content_bb, legend_bb;
    bool legend_hovered = false;
    if (plot.showLegend && nItems > 0)
    {
        float maxLabelWidth = 0;
        for (int i = 0; i < nItems; ++i)
        {
            auto labelWidth = CalcTextSize(items[i].label.c_str());
            maxLabelWidth = labelWidth.x > maxLabelWidth ? labelWidth.x : maxLabelWidth;
        }
        legend_content_bb = ImRect(grid_bb.Min + legend_offset, grid_bb.Min + legend_offset + ImVec2(maxLabelWidth, nItems * textHeight));
        legend_bb = ImRect(legend_content_bb.Min, legend_content_bb.Max + legend_padding * 2 + ImVec2(legend_icon_size, 0));
        legend_hovered = frame_hovered && legend_bb.Contains(IO.MousePos);
    }

    // end drag
    if (plot._dragging && (IO.MouseReleased[0] || !IO.MouseDown[0]))
    {
        plot._dragging = false;
    }
    // drag
    if (plot._dragging)
    {
        bool xLocked = plot.xAxis.lockMin || plot.xAxis.lockMax;
        if (!xLocked)
        {
            float dir = plot.xAxis.flip ? -1.0f : 1.0f;
            float delX = dir * IO.MouseDelta.x * (plot.xAxis.maximum - plot.xAxis.minimum) / (grid_bb.Max.x - grid_bb.Min.x);
            plot.xAxis.minimum -= delX;
            plot.xAxis.maximum -= delX;
        }
        bool yLocked = plot.yAxis.lockMin || plot.yAxis.lockMax;
        if (!yLocked)
        {
            float dir = plot.yAxis.flip ? -1.0f : 1.0f;
            float delY = dir * IO.MouseDelta.y * (plot.yAxis.maximum - plot.yAxis.minimum) / (grid_bb.Max.y - grid_bb.Min.y);
            plot.yAxis.minimum += delY;
            plot.yAxis.maximum += delY;
        }
        if (xLocked && yLocked)
            ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
        else if (xLocked)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        else if (yLocked)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    // start drag
    if (frame_hovered && grid_hovered && IO.MouseClicked[0] && !plot._selecting &&!legend_hovered)
    {
        plot._dragging = true;
    }
    // scroll zoom
    if (frame_hovered && (xAxisRegion_hovered || yAxisRegion_hovered))
    {
        float xRange = plot.xAxis.maximum - plot.xAxis.minimum;
        float yRange = plot.yAxis.maximum - plot.yAxis.minimum;
        if (IO.MouseWheel < 0)
        {
            if (xAxisRegion_hovered) {
                if (!plot.xAxis.lockMin)
                    plot.xAxis.minimum -= plot.xAxis.zoomRate * xRange;
                if (!plot.xAxis.lockMax)
                    plot.xAxis.maximum += plot.xAxis.zoomRate * xRange;
            }
            if (yAxisRegion_hovered) {
                if (!plot.yAxis.lockMin)
                    plot.yAxis.minimum -= plot.yAxis.zoomRate * yRange;
                if (!plot.yAxis.lockMax)
                    plot.yAxis.maximum += plot.yAxis.zoomRate * yRange;
            }
        }
        if (IO.MouseWheel > 0)
        {
            if (xAxisRegion_hovered) {
                if (!plot.xAxis.lockMin)
                    plot.xAxis.minimum += plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
                if (!plot.xAxis.lockMax)
                    plot.xAxis.maximum -= plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
            }
            if (yAxisRegion_hovered) {
                if (!plot.yAxis.lockMin)
                    plot.yAxis.minimum += plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
                if (!plot.yAxis.lockMax)
                    plot.yAxis.maximum -= plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
            }
        }
    }

    // get pixels for transforms
    const ImRect pix(plot.xAxis.flip ? grid_bb.Max.x : grid_bb.Min.x,
                     plot.yAxis.flip ? grid_bb.Min.y : grid_bb.Max.y,
                     plot.xAxis.flip ? grid_bb.Min.x : grid_bb.Max.x,
                     plot.yAxis.flip ? grid_bb.Max.y : grid_bb.Min.y);

    // confirm selection
    if (plot._selecting && (IO.MouseReleased[1] || !IO.MouseDown[1]))
    {
        ImVec2 slcSize = plot._selectStart - IO.MousePos;
        if (std::abs(slcSize.x) > 5 && std::abs(slcSize.y) > 5)
        {
            ImVec2 p1, p2;
            p1.x = Remap(plot._selectStart.x, pix.Min.x, pix.Max.x, plot.xAxis.minimum, plot.xAxis.maximum);
            p1.y = Remap(plot._selectStart.y, pix.Min.y, pix.Max.y, plot.yAxis.minimum, plot.yAxis.maximum);
            p2.x = Remap(IO.MousePos.x, pix.Min.x, pix.Max.x, plot.xAxis.minimum, plot.xAxis.maximum);
            p2.y = Remap(IO.MousePos.y, pix.Min.y, pix.Max.y, plot.yAxis.minimum, plot.yAxis.maximum);
            plot.xAxis.minimum = ImMin(p1.x, p2.x);
            plot.xAxis.maximum = ImMax(p1.x, p2.x);
            plot.yAxis.minimum = ImMin(p1.y, p2.y);
            plot.yAxis.maximum = ImMax(p1.y, p2.y);
        }
        plot._selecting = false;
    }
    // cancel selection
    if (plot._selecting && (IO.MouseClicked[0] || IO.MouseDown[0]))
    {
        plot._selecting = false;
    }
    if (frame_hovered && grid_hovered && IO.MouseClicked[1] && plot.enableSelection)
    {
        plot._selectStart = IO.MousePos;
        plot._selecting = true;
    }

    // RENDER

    // grid bg
    DrawList.AddRectFilled(grid_bb.Min, grid_bb.Max, color_bg);

    // render axes
    DrawList.PushClipRect(grid_bb.Min, grid_bb.Max);

    // transform ticks
    if (renderX)
        TransformTicks(xTicks, plot.xAxis.minimum, plot.xAxis.maximum, pix.Min.x, pix.Max.x);
    if (renderY)
        TransformTicks(yTicks, plot.yAxis.minimum, plot.yAxis.maximum, pix.Min.y, pix.Max.y);

    // render grid
    if (plot.xAxis.showGrid)
    {
        for (auto &xt : xTicks)
            DrawList.AddLine({xt.pixels, grid_bb.Min.y}, {xt.pixels, grid_bb.Max.y}, xt.major ? color_x1 : color_x2, 1);
    }

    if (plot.yAxis.showGrid)
    {
        for (auto &yt : yTicks)
            DrawList.AddLine({grid_bb.Min.x, yt.pixels}, {grid_bb.Max.x, yt.pixels}, yt.major ? color_y1 : color_y2, 1);
    }

    // render plot items
    for (int i = 0; i < nItems; ++i) {
        if (items[i].show && items[i].data.size() > 0) {
            if (items[i].type == PlotItem::Line)
                RenderPlotItemLine(items[i], plot, pix, DrawList);
            else if (items[i].type == PlotItem::Scatter)
                RenderPlotItemScatter(items[i], plot, pix, DrawList);
            else if (items[i].type == PlotItem::XBar)
                RenderPlotItemXBar(items[i], plot, pix, DrawList);
            else if (items[i].type == PlotItem::YBar)
                RenderPlotItemYBar(items[i], plot, pix, DrawList);
        }
    }

    // render selection
    if (plot._selecting)
    {
        DrawList.AddRectFilled(ImMin(IO.MousePos, plot._selectStart), ImMax(IO.MousePos, plot._selectStart), color_slctBg);
        DrawList.AddRect(ImMin(IO.MousePos, plot._selectStart), ImMax(IO.MousePos, plot._selectStart), color_slctBd);
    }

    // render ticks
    if (plot.xAxis.showTickMarks)
    {
        for (auto &xt : xTicks)
            DrawList.AddLine({xt.pixels, grid_bb.Max.y}, {xt.pixels, grid_bb.Max.y - (xt.major ? 10.0f : 5.0f)}, color_border, 1);
    }
    if (plot.yAxis.showTickMarks)
    {
        for (auto &yt : yTicks)
            DrawList.AddLine({grid_bb.Min.x, yt.pixels}, {grid_bb.Min.x + (yt.major ? 10.0f : 5.0f), yt.pixels}, color_border, 1);
    }

    // render crosshairs
    if (plot.showCrosshairs && grid_hovered && frame_hovered && !plot._dragging && !plot._selecting && !legend_hovered)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        ImVec2 xy = IO.MousePos;
        ImVec2 h1(grid_bb.Min.x, xy.y);
        ImVec2 h2(xy.x - 5, xy.y);
        ImVec2 h3(xy.x + 5, xy.y);
        ImVec2 h4(grid_bb.Max.x, xy.y);
        ImVec2 v1(xy.x, grid_bb.Min.y);
        ImVec2 v2(xy.x, xy.y - 5);
        ImVec2 v3(xy.x, xy.y + 5);
        ImVec2 v4(xy.x, grid_bb.Max.y);
        DrawList.AddLine(h1, h2, color_border);
        DrawList.AddLine(h3, h4, color_border);
        DrawList.AddLine(v1, v2, color_border);
        DrawList.AddLine(v3, v4, color_border);
    }

    // render legend
    if (plot.showLegend && nItems > 0)
    {
        DrawList.AddRectFilled(legend_bb.Min, legend_bb.Max, GetColorU32(ImGuiCol_PopupBg));
        DrawList.AddRect(legend_bb.Min, legend_bb.Max, color_border);
        for (int i = 0; i < nItems; ++i)
        {           
            ImRect icon_bb;
            icon_bb.Min = legend_content_bb.Min + legend_padding + ImVec2(0, i * textHeight) + ImVec2(2, 2);
            icon_bb.Max = legend_content_bb.Min + legend_padding + ImVec2(0, i * textHeight) + ImVec2(legend_icon_size - 2, legend_icon_size - 2);
            ImU32 iconColor;
            if (legend_hovered && icon_bb.Contains(IO.MousePos)) {
                auto colAlpha = items[i].color;
                colAlpha.w = 0.5f;
                iconColor = items[i].show ? GetColorU32(colAlpha) : GetColorU32(ImGuiCol_TextDisabled, 0.5f);
                if (IO.MouseClicked[0])
                    items[i].show = !items[i].show;
            }
            else {
                iconColor = items[i].show ? GetColorU32(items[i].color) : color_txt_dis;
            }
            DrawList.AddRectFilled(icon_bb.Min, icon_bb.Max, iconColor, 1);
            DrawList.AddText(legend_content_bb.Min + legend_padding + ImVec2(legend_icon_size, i * textHeight), items[i].show ? color_txt : color_txt_dis, items[i].label.c_str());
            
        }
    }

    // render mouse pos
    if (plot.showMousePos && grid_hovered)
    {
        static char buffer[32];
        ImVec2 posPlt;
        posPlt.x = Remap(IO.MousePos.x, pix.Min.x, pix.Max.x, plot.xAxis.minimum, plot.xAxis.maximum);
        posPlt.y = Remap(IO.MousePos.y, pix.Min.y, pix.Max.y, plot.yAxis.minimum, plot.yAxis.maximum);
        sprintf(buffer, "%.2f,%.2f", posPlt.x, posPlt.y);
        ImVec2 size = CalcTextSize(buffer);
        ImVec2 pos = grid_bb.Max - size - ImVec2(textOffset, textOffset);
        DrawList.AddText(pos, color_txt, buffer);
    }

    DrawList.PopClipRect();

    // render border
    DrawList.AddRect(grid_bb.Min, grid_bb.Max, color_border);

    // render title
    if (plot.title != "") {
        const ImVec2 title_size = CalcTextSize(plot.title.c_str());
        DrawList.AddText(ImVec2(canvas_bb.GetCenter().x - title_size.x * 0.5f, canvas_bb.Min.y), color_txt, plot.title.c_str());
    }

    // render labels
    if (plot.xAxis.showTickLabels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &xt : xTicks)
            DrawList.AddText({xt.pixels - xt.size.x * 0.5f, grid_bb.Max.y + textOffset}, color_xtxt, xt.txt.c_str());
        DrawList.PopClipRect();
    }
    if (plot.xAxis.label != "") {
        const ImVec2 xLabel_size = CalcTextSize(plot.xAxis.label.c_str());
        const ImVec2 xLabel_pos(grid_bb.GetCenter().x - xLabel_size.x * 0.5f, canvas_bb.Max.y - textHeight);
        DrawList.AddText(xLabel_pos, color_xtxt, plot.xAxis.label.c_str());
    }
    if (plot.yAxis.showTickLabels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &yt : yTicks)
            DrawList.AddText({grid_bb.Min.x - textOffset - yt.size.x, yt.pixels - 0.5f * yt.size.y}, color_ytxt, yt.txt.c_str());
        DrawList.PopClipRect();
    }
    if (plot.yAxis.label != "") {
        const ImVec2 yLabel_size = CalcTextSizeVertical(plot.yAxis.label.c_str());
        const ImVec2 yLabel_pos(canvas_bb.Min.x, grid_bb.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, plot.yAxis.label.c_str(), yLabel_pos, color_ytxt);
    }

    // AddTextVertical(&DrawList, "Hello, World", frame_bb.Min, color_ytxt);

    PushID(id);
    if (frame_hovered && grid_hovered && IO.MouseClicked[2] && plot.enableControls && !legend_hovered)
        ImGui::OpenPopup("##Context");
    if (ImGui::BeginPopup("##Context"))
    {
        ImGui::PushItemWidth(100);
        ImGui::BeginGroup();
        ImGui::LabelText("##X-Axis", "X-Axis");
        ImGui::DragFloat("##Xmin", &plot.xAxis.minimum, 0.01f + 0.01f * (plot.xAxis.maximum - plot.xAxis.minimum), -INFINITY, plot.xAxis.maximum - FLT_EPSILON);
        ImGui::SameLine();
        ImGui::ToggleButton((plot.xAxis.lockMin ? ICON_FA_LOCK "##Xmin" : ICON_FA_LOCK_OPEN "##Xmin"), &plot.xAxis.lockMin, {20, 0});
        ImGui::DragFloat("##Xmax", &plot.xAxis.maximum, 0.01f + 0.01f * (plot.xAxis.maximum - plot.xAxis.minimum), plot.xAxis.minimum + FLT_EPSILON, INFINITY);
        ImGui::SameLine();
        ImGui::ToggleButton((plot.xAxis.lockMax ? ICON_FA_LOCK "##Xmax" : ICON_FA_LOCK_OPEN "##Xmax"), &plot.xAxis.lockMax, {20, 0});
        ImGui::ToggleButton(ICON_FA_ARROWS_ALT_H, &plot.xAxis.flip, ImVec2(20, 0));
        ImGui::SameLine();
        ImGui::ToggleButton("01##X", &plot.xAxis.showTickLabels, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_GRIP_LINES_VERTICAL, &plot.xAxis.showGrid, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_ELLIPSIS_H, &plot.xAxis.showTickMarks, ImVec2(23, 0));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::LabelText("##Y-Axis", "Y-Axis");
        ImGui::DragFloat("##Ymin", &plot.yAxis.minimum, 0.01f + 0.01f * (plot.yAxis.maximum - plot.yAxis.minimum), -INFINITY, plot.yAxis.maximum - FLT_EPSILON);
        ImGui::SameLine();
        ImGui::ToggleButton((plot.yAxis.lockMin ? ICON_FA_LOCK "##Ymin" : ICON_FA_LOCK_OPEN "##Ymin"), &plot.yAxis.lockMin, {20, 0});
        ImGui::DragFloat("##Ymax", &plot.yAxis.maximum, 0.01f + 0.01f * (plot.yAxis.maximum - plot.yAxis.minimum), plot.yAxis.minimum + FLT_EPSILON, INFINITY);
        ImGui::SameLine();
        ImGui::ToggleButton((plot.yAxis.lockMax ? ICON_FA_LOCK "##Ymax" : ICON_FA_LOCK_OPEN "##Ymax"), &plot.yAxis.lockMax, {20, 0});
        ImGui::ToggleButton(ICON_FA_ARROWS_ALT_V, &plot.yAxis.flip, ImVec2(20, 0));
        ImGui::SameLine();
        ImGui::ToggleButton("01##Y", &plot.yAxis.showTickLabels, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_GRIP_LINES, &plot.yAxis.showGrid, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_ELLIPSIS_V, &plot.yAxis.showTickMarks, ImVec2(23, 0));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(25);
        ImGui::LabelText("##", "");
        ImGui::LabelText("##Min", "Min");
        ImGui::LabelText("##Max", "Max");
        ImGui::PopItemWidth();
        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::ToggleButton(ICON_FA_CROSSHAIRS, &plot.showCrosshairs, {30, 0});
        ImGui::SameLine();
        ImGui::ToggleButton("X,Y", &plot.showMousePos, {30, 0});
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_VECTOR_SQUARE, &plot.enableSelection, {30, 0});
        ImGui::SameLine();
        ImGui::ToggleButton(ICON_FA_LIST, &plot.showLegend, {30, 0});
        ImGui::EndPopup();
    }
    PopID();

#if 0
    DrawList.AddRect(legend_bb.Min, legend_bb.Max, GetColorU32({0,1,0,1}));
    DrawList.AddRect(canvas_bb.Min, canvas_bb.Max, GetColorU32({1,0,0,1}));
    DrawList.AddRect(xAxisRegion_bb.Min, xAxisRegion_bb.Max, GetColorU32({1,0,1,1}));
    DrawList.AddRect(yAxisRegion_bb.Min, yAxisRegion_bb.Max, GetColorU32({1,1,0,1}));
#endif
}

}