#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <mahi/gui/imgui_custom.hpp>
#include <imgui_internal.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <mahi/gui/Print.hpp>
#include <mahi/gui/Icons/IconsFontAwesome5.hpp>
#include <mahi/gui/Color.hpp>

namespace ImGui
{

void EnableViewports()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

void DisableViewports()
{
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
}

void EnableDocking()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void DisableDocking()
{
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
}

bool BeginFixed(const char *name, const ImVec2 &pos, const ImVec2 &size, ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    return ImGui::Begin(name, nullptr, flags);
}

void HoverTooltip(const char *tip, float delay)
{
    if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay)
    {
        ImGui::SetTooltip("%s", tip);
    }
}

void BeginDisabled(bool disabled, float alpha)
{
    if (disabled)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * alpha);
    }
}

/// Ends a disabled section
void EndDisabled(bool disabled)
{
    if (disabled)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void EnableButton(const char *label, bool *enabled, const ImVec2 &size)
{
    bool dim = !*enabled;
    if (dim)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
    if (ImGui::Button(label, size))
        *enabled = !(*enabled);
    if (dim)
        ImGui::PopStyleVar();
}

bool ButtonColored(const char *label, const ImVec4 &color, const ImVec2 &size)
{
    ImVec4 colorHover = ImLerp({1, 1, 1, color.w}, color, 0.8f);
    ImVec4 colorPress = ImLerp({0, 0, 0, color.w}, color, 0.8f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorPress);
    bool ret = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return ret;
}

bool ModeSelector(int* currentMode, const std::vector<std::string>& modes, bool horizontal) {
    bool changed = false;
    for (int i = 0; i < modes.size(); ++i) {
        if (ImGui::RadioButton(modes[i].c_str(), *currentMode == i)) {
            *currentMode = i;
            changed = true;
        }
        if (horizontal && i+1 != modes.size())
            ImGui::SameLine();
    }
    return changed;
}

///////////////////////////////////////////////////////////////////////////////
// DOUBLES
///////////////////////////////////////////////////////////////////////////////

bool DragDouble(const char *label, double *v, float v_speed, double v_min, double v_max, const char *format, float power)
{
    return DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, format, power);
}

bool DragDouble2(const char *label, double v[2], float v_speed, double v_min, double v_max, const char *format, float power)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 2, v_speed, &v_min, &v_max, format, power);
}

bool DragDouble3(const char *label, double v[3], float v_speed, double v_min, double v_max, const char *format, float power)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 3, v_speed, &v_min, &v_max, format, power);
}

bool DragDouble4(const char *label, double v[4], float v_speed, double v_min, double v_max, const char *format, float power)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 4, v_speed, &v_min, &v_max, format, power);
}

bool DragDoubleRange2(const char *label, double *v_current_min, double *v_current_max, float v_speed, double v_min, double v_max, const char *format, const char *format_max, float power)
{
    ImGuiWindow *window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    bool value_changed = DragDouble("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragDouble("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, format_max ? format_max : format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

bool SliderDouble(const char *label, double *v, double v_min, double v_max, const char *format, float power)
{
    return SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, power);
}

bool SliderDouble2(const char *label, double v[2], double v_min, double v_max, const char *format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Double, v, 2, &v_min, &v_max, format, power);
}

bool SliderDouble3(const char *label, double v[3], double v_min, double v_max, const char *format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Double, v, 3, &v_min, &v_max, format, power);
}

bool SliderDouble4(const char *label, double v[4], double v_min, double v_max, const char *format, float power)
{
    return SliderScalarN(label, ImGuiDataType_Double, v, 4, &v_min, &v_max, format, power);
}

bool InputDouble2(const char *label, double v[2], const char *format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Double, v, 2, NULL, NULL, format, flags);
}

bool InputDouble3(const char *label, double v[3], const char *format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Double, v, 3, NULL, NULL, format, flags);
}

bool InputDouble4(const char *label, double v[4], const char *format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Double, v, 4, NULL, NULL, format, flags);
}

///////////////////////////////////////////////////////////////////////////////
// PLOT
//////////////////////////////////////////////////////////////////////////////

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

PlotAxis::PlotAxis() : showGrid(true), showTicks(true), showLabels(true), minimum(0), maximum(1), divisions(4), subDivisions(5),
                       color(0,0,0,-1),
                       zoomRate(0.1f), lockMin(false), lockMax(false), flip(false)
{
}

PlotInterface::PlotInterface() : showCrosshairs(false), showMousePos(true), showLegend(true), enableSelection(true), enableControls(true), _dragging(false), _selecting(false)
{
    frameColor = ImVec4(0,0,0,-1);
    backgroundColor = ImVec4(0,0,0,-1);
    borderColor = ImVec4(0,0,0,-1);
    selectionColor = {.118f, .565f, 1, 0.25};
}

bool Plot(const char *label_id, PlotInterface &plot, const std::vector<PlotItem> &items, const ImVec2 &size)
{
    return Plot(label_id, &plot, &items[0], items.size(), size);
}

bool Plot(const char *label_id, PlotInterface *plot_ptr, const PlotItem *items, int nItems, const ImVec2 &size)
{
    PlotInterface &plot = *plot_ptr;
    // ImGui front matter
    ImGuiWindow *Window = GetCurrentWindow();
    if (Window->SkipItems)
        return false;
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

    const ImVec4 color_yAxis = plot.xAxis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.xAxis.color;
    const ImU32 color_y1 = GetColorU32(color_yAxis);
    const ImU32 color_y2 = GetColorU32(color_yAxis * ImVec4(1, 1, 1, 0.25f));
    const ImU32 color_ytxt = GetColorU32({color_yAxis.x, color_yAxis.y, color_yAxis.z, 1});

    const ImU32 color_txt = GetColorU32(ImGuiCol_Text);
    const ImU32 color_slctBg = GetColorU32(plot.selectionColor);
    const ImU32 color_slctBd = GetColorU32({plot.selectionColor.x, plot.selectionColor.y, plot.selectionColor.z, 1});

    // constrain axes
    if (plot.xAxis.maximum <= plot.xAxis.minimum)
        plot.xAxis.maximum = plot.xAxis.minimum + std::numeric_limits<float>::epsilon();
    if (plot.yAxis.maximum <= plot.yAxis.minimum)
        plot.yAxis.maximum = plot.yAxis.minimum + std::numeric_limits<float>::epsilon();

    // get ticks
    static std::vector<Tick> xTicks(100), yTicks(100);
    const bool renderX = (plot.xAxis.showGrid || plot.xAxis.showTicks || plot.xAxis.showLabels) && plot.xAxis.divisions > 1;
    const bool renderY = (plot.yAxis.showGrid || plot.yAxis.showTicks || plot.yAxis.showLabels) && plot.yAxis.divisions > 1;

    if (renderX)
        GetTicks(plot.xAxis.minimum, plot.xAxis.maximum, plot.xAxis.divisions, plot.xAxis.subDivisions, xTicks);
    if (renderY)
        GetTicks(plot.yAxis.minimum, plot.yAxis.maximum, plot.yAxis.divisions, plot.yAxis.subDivisions, yTicks);

    // label ticks
    if (plot.xAxis.showLabels)
        LabelTicks(xTicks);
    if (plot.yAxis.showLabels)
        LabelTicks(yTicks);

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    const ImRect frame_bb(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(frame_bb);
    if (!ItemAdd(frame_bb, 0, &frame_bb))
        return false;
    const bool frame_hovered = ItemHoverable(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, color_frame, true, Style.FrameRounding);

    // canvas bb
    const ImRect canvas_bb(frame_bb.Min + Style.WindowPadding, frame_bb.Max - Style.WindowPadding);

    // get max y-tick width
    float maxLabelWidth = 0;
    if (plot.yAxis.showLabels)
    {
        for (auto &yt : yTicks)
            maxLabelWidth = yt.size.x > maxLabelWidth ? yt.size.x : maxLabelWidth;
    }

    // grid bb
    const float textOffset = 5;
    const float bPadding = plot.xAxis.showLabels ? GetTextLineHeight() + textOffset : 0;
    const float lPadding = plot.yAxis.showLabels ? maxLabelWidth + textOffset : 0;
    const ImRect grid_bb(canvas_bb.Min + ImVec2(lPadding, 0), canvas_bb.Max - ImVec2(0, bPadding));
    const bool grid_hovered = grid_bb.Contains(IO.MousePos);

    // axis label bb
    const ImRect xLabel_bb(grid_bb.GetBL(), canvas_bb.Max);
    const ImRect yLabel_bb(frame_bb.Min, grid_bb.GetBL());

    // CONTROLS
    bool changed = false;

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
    if (frame_hovered && grid_hovered && IO.MouseClicked[0] && !plot._selecting)
    {
        plot._dragging = true;
    }
    // scroll zoom
    if (frame_hovered && grid_hovered)
    {
        float xRange = plot.xAxis.maximum - plot.xAxis.minimum;
        float yRange = plot.yAxis.maximum - plot.yAxis.minimum;
        if (IO.MouseWheel < 0)
        {
            if (!plot.xAxis.lockMin)
                plot.xAxis.minimum -= plot.xAxis.zoomRate * xRange;
            if (!plot.xAxis.lockMax)
                plot.xAxis.maximum += plot.xAxis.zoomRate * xRange;
            if (!plot.yAxis.lockMin)
                plot.yAxis.minimum -= plot.yAxis.zoomRate * yRange;
            if (!plot.yAxis.lockMax)
                plot.yAxis.maximum += plot.yAxis.zoomRate * yRange;
        }
        if (IO.MouseWheel > 0)
        {
            if (!plot.xAxis.lockMin)
                plot.xAxis.minimum += plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
            if (!plot.xAxis.lockMax)
                plot.xAxis.maximum -= plot.xAxis.zoomRate * xRange / (1.0f + 2.0f * plot.xAxis.zoomRate);
            if (!plot.yAxis.lockMin)
                plot.yAxis.minimum += plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
            if (!plot.yAxis.lockMax)
                plot.yAxis.maximum -= plot.yAxis.zoomRate * yRange / (1.0f + 2.0f * plot.yAxis.zoomRate);
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
    if (plot.xAxis.showTicks)
    {
        for (auto &xt : xTicks)
            DrawList.AddLine({xt.pixels, grid_bb.Max.y}, {xt.pixels, grid_bb.Max.y - (xt.major ? 10.0f : 5.0f)}, color_border, 1);
    }
    if (plot.yAxis.showTicks)
    {
        for (auto &yt : yTicks)
            DrawList.AddLine({grid_bb.Min.x, yt.pixels}, {grid_bb.Min.x + (yt.major ? 10.0f : 5.0f), yt.pixels}, color_border, 1);
    }

    // render crosshairs
    if (plot.showCrosshairs && grid_hovered && frame_hovered && !plot._dragging && !plot._selecting)
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
        const ImVec2 legend_offset(10, 10);
        const ImVec2 legend_padding(5, 5);
        float maxLabelWidth = 0;
        float labelHeight = GetTextLineHeight();
        float iconSize = labelHeight;
        for (int i = 0; i < nItems; ++i)
        {
            auto labelWidth = CalcTextSize(items[i].label.c_str());
            maxLabelWidth = labelWidth.x > maxLabelWidth ? labelWidth.x : maxLabelWidth;
        }
        ImRect legend_bb(grid_bb.Min + legend_offset, grid_bb.Min + legend_offset + ImVec2(maxLabelWidth, nItems * labelHeight));
        DrawList.AddRectFilled(legend_bb.Min, legend_bb.Max + legend_padding * 2 + ImVec2(iconSize, 0), GetColorU32(ImGuiCol_PopupBg));
        DrawList.AddRect(legend_bb.Min, legend_bb.Max + legend_padding * 2 + ImVec2(iconSize, 0), color_border);
        for (int i = 0; i < nItems; ++i)
        {
            auto iconColor = GetColorU32(items[i].color);
            ImVec2 iconMin = legend_bb.Min + legend_padding + ImVec2(0, i * labelHeight);
            DrawList.AddRectFilled(iconMin + ImVec2(2, 2), iconMin + ImVec2(iconSize - 2, iconSize - 2), iconColor, 1);
            DrawList.AddText(legend_bb.Min + legend_padding + ImVec2(iconSize, i * labelHeight), color_txt, items[i].label.c_str());
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

    // render labels
    if (plot.xAxis.showLabels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &xt : xTicks)
            DrawList.AddText({xt.pixels - xt.size.x * 0.5f, grid_bb.Max.y + textOffset}, color_xtxt, xt.txt.c_str());
        DrawList.PopClipRect();
    }
    if (plot.yAxis.showLabels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &yt : yTicks)
            DrawList.AddText({grid_bb.Min.x - textOffset - yt.size.x, yt.pixels - 0.5f * yt.size.y}, color_ytxt, yt.txt.c_str());
        DrawList.PopClipRect();
    }

    PushID(id);
    if (frame_hovered && grid_hovered && IO.MouseClicked[2] && plot.enableControls)
        ImGui::OpenPopup("##Context");
    if (ImGui::BeginPopup("##Context"))
    {
        ImGui::PushItemWidth(100);
        ImGui::BeginGroup();
        ImGui::LabelText("##X-Axis", "X-Axis");
        ImGui::DragFloat("##Xmin", &plot.xAxis.minimum, 0.01f + 0.01f * (plot.xAxis.maximum - plot.xAxis.minimum), -INFINITY, plot.xAxis.maximum - FLT_EPSILON);
        ImGui::SameLine();
        ImGui::EnableButton((plot.xAxis.lockMin ? ICON_FA_LOCK "##Xmin" : ICON_FA_LOCK_OPEN "##Xmin"), &plot.xAxis.lockMin, {20, 0});
        ImGui::DragFloat("##Xmax", &plot.xAxis.maximum, 0.01f + 0.01f * (plot.xAxis.maximum - plot.xAxis.minimum), plot.xAxis.minimum + FLT_EPSILON, INFINITY);
        ImGui::SameLine();
        ImGui::EnableButton((plot.xAxis.lockMax ? ICON_FA_LOCK "##Xmax" : ICON_FA_LOCK_OPEN "##Xmax"), &plot.xAxis.lockMax, {20, 0});
        ImGui::EnableButton(ICON_FA_ARROWS_ALT_H, &plot.xAxis.flip, ImVec2(20, 0));
        ImGui::SameLine();
        ImGui::EnableButton("01##X", &plot.xAxis.showLabels, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_GRIP_LINES_VERTICAL, &plot.xAxis.showGrid, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_ELLIPSIS_H, &plot.xAxis.showTicks, ImVec2(23, 0));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::LabelText("##Y-Axis", "Y-Axis");
        ImGui::DragFloat("##Ymin", &plot.yAxis.minimum, 0.01f + 0.01f * (plot.yAxis.maximum - plot.yAxis.minimum), -INFINITY, plot.yAxis.maximum - FLT_EPSILON);
        ImGui::SameLine();
        ImGui::EnableButton((plot.yAxis.lockMin ? ICON_FA_LOCK "##Ymin" : ICON_FA_LOCK_OPEN "##Ymin"), &plot.yAxis.lockMin, {20, 0});
        ImGui::DragFloat("##Ymax", &plot.yAxis.maximum, 0.01f + 0.01f * (plot.yAxis.maximum - plot.yAxis.minimum), plot.yAxis.minimum + FLT_EPSILON, INFINITY);
        ImGui::SameLine();
        ImGui::EnableButton((plot.yAxis.lockMax ? ICON_FA_LOCK "##Ymax" : ICON_FA_LOCK_OPEN "##Ymax"), &plot.yAxis.lockMax, {20, 0});
        ImGui::EnableButton(ICON_FA_ARROWS_ALT_V, &plot.yAxis.flip, ImVec2(20, 0));
        ImGui::SameLine();
        ImGui::EnableButton("01##Y", &plot.yAxis.showLabels, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_GRIP_LINES, &plot.yAxis.showGrid, ImVec2(23, 0));
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_ELLIPSIS_V, &plot.yAxis.showTicks, ImVec2(23, 0));
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
        ImGui::EnableButton(ICON_FA_CROSSHAIRS, &plot.showCrosshairs, {30, 0});
        ImGui::SameLine();
        ImGui::EnableButton("X,Y", &plot.showMousePos, {30, 0});
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_VECTOR_SQUARE, &plot.enableSelection, {30, 0});
        ImGui::SameLine();
        ImGui::EnableButton(ICON_FA_LIST, &plot.showLegend, {30, 0});
        ImGui::EndPopup();
    }
    PopID();

#if 0
    DrawList.AddRect(canvas_bb.Min, canvas_bb.Max, GetColorU32({1,0,0,1}));
    DrawList.AddRect(xLabel_bb.Min, xLabel_bb.Max, GetColorU32({1,0,1,1}));
    DrawList.AddRect(yLabel_bb.Min, yLabel_bb.Max, GetColorU32({1,0,1,1}));
#endif

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// STYLES
///////////////////////////////////////////////////////////////////////////////

void StyleColorsMahiDark1()
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
    colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void StyleColorsMahiDark2()
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
}

void StyleColorsMahiDark3()
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void StyleColorsMahiDark4() {
    ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

	#ifdef IMGUI_HAS_DOCK 
		colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			colors[ImGuiCol_WindowBg].w = 1.0f;		
	#endif
}

} // namespace ImGui