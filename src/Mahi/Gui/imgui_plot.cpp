#ifndef IMGUI_DEFINE_MATH_OPERATORS
    #define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <Mahi/Gui/imgui_plot.hpp>
#include <imgui_internal.h>
#include <sstream>
#include <cmath>
#include <iostream>
#include <algorithm>

// uncomment this to use AA on Line plots (considerably slower)
// #define IMGUI_PLOT_LINE_USE_AA

#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)     { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } }

namespace ImGui {

// private namespace
namespace
{

/// Global state vars
ImVec2 g_plot_mouse_pos = {0,0};
bool   g_plot_area_hovered   = false;

/// Linearly remaps float x from [x0 x1] to [y0 y1].
inline float Remap(float x, float x0, float x1, float y0, float y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

/// Utility function to that rounds x to powers of 2,5 and 10 for generating axis labels
/// Taken from Graphics Gems 1 Chapter 11.2, "Nice Numbers for Graph Labels"
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
/// Draws vertical text. The position is the bottom left of the text rect.
inline void AddTextVertical(ImDrawList* DrawList, const char *text, ImVec2 pos, ImU32 text_color) {
    pos.x = IM_ROUND(pos.x);
    pos.y = IM_ROUND(pos.y);
    ImFont *font = GImGui->Font;
    const ImFontGlyph *glyph;
    char c;
    while ((c = *text++)) {
        glyph = font->FindGlyph(c);
        if (!glyph) continue;

        DrawList->PrimReserve(6, 4);
        DrawList->PrimQuadUV(
                pos + ImVec2(glyph->Y0, -glyph->X0),
                pos + ImVec2(glyph->Y0, -glyph->X1),
                pos + ImVec2(glyph->Y1, -glyph->X1),
                pos + ImVec2(glyph->Y1, -glyph->X0),

                ImVec2(glyph->U0, glyph->V0),
                ImVec2(glyph->U1, glyph->V0),
                ImVec2(glyph->U1, glyph->V1),
                ImVec2(glyph->U0, glyph->V1),
                    text_color);
        pos.y -= glyph->AdvanceX;
    }
}

/// Calculates the size of vertical text
inline ImVec2 CalcTextSizeVertical(const char* text) {
    ImVec2 sz = CalcTextSize(text);
    return ImVec2(sz.y, sz.x);
}

/// Tick mark info
struct Tick {
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
    // const int nfrac = std::max((int)-floor(log10(interval)), 0);
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
        tk.pixels = (pMin + m * ((float)tk.plot - tMin)); // IM_ROUND used previoiusly, but causes distortion in moving plot
}

inline void RenderPlotItemLineAA(const PlotItem& item, const PlotInterface& plot, const ImRect& pix, ImDrawList& DrawList) {
    if (item.data.size() < 2)
        return;
    static std::vector<ImVec2> pointsPx(10000); // up front allocation
    pointsPx.resize(item.data.size());
    const float mx = (pix.Max.x - pix.Min.x) / (plot.x_axis.maximum - plot.x_axis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.y_axis.maximum - plot.y_axis.minimum);
    // transform data
    for (std::size_t i = 0; i < item.data.size(); ++i)
    {
        pointsPx[i].x = pix.Min.x + mx * (item.data[i].x - plot.x_axis.minimum);
        pointsPx[i].y = pix.Min.y + my * (item.data[i].y - plot.y_axis.minimum);
    }
    const ImU32 color = GetColorU32(item.color);
    std::size_t segments = item.data.size() - 1;
    std::size_t i = (std::size_t)item.data_begin;
    for (std::size_t s = 0; s < segments; ++s) {
        std::size_t j = i + 1;
        if (j == item.data.size())
            j = 0;
        DrawList.AddLine(pointsPx[i], pointsPx[j], color, item.size);
        i = j;
    }
}

inline void RenderPlotItemLine(const PlotItem& item, const PlotInterface& plot, const ImRect& pix, ImDrawList& DrawList) {
    if (item.data.size() < 2)
        return;
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.x_axis.maximum - plot.x_axis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.y_axis.maximum - plot.y_axis.minimum);
    const int points_count = (int)item.data.size();
    const int segments = (int)item.data.size() - 1;
    const int idx_count = segments*6;
    const int vtx_count = segments*4;  
    const float thickness = item.size;
    const ImVec2 uv = DrawList._Data->TexUvWhitePixel;
    DrawList.PrimReserve(idx_count, vtx_count);
    int i1 = item.data_begin;
    ImVec2 p1, p2;
    for (int s = 0; s < segments; ++s) {
        const int i2 = i1 + 1 == points_count ? 0 : i1 + 1;
        p1.x = pix.Min.x + mx * (item.data[i1].x - plot.x_axis.minimum);
        p1.y = pix.Min.y + my * (item.data[i1].y - plot.y_axis.minimum);
        p2.x = pix.Min.x + mx * (item.data[i2].x - plot.x_axis.minimum);
        p2.y = pix.Min.y + my * (item.data[i2].y - plot.y_axis.minimum);
        i1 = i2;

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        IM_NORMALIZE2F_OVER_ZERO(dx, dy);
        dx *= (thickness * 0.5f);
        dy *= (thickness * 0.5f);

        DrawList._VtxWritePtr[0].pos.x = p1.x + dy; DrawList._VtxWritePtr[0].pos.y = p1.y - dx; DrawList._VtxWritePtr[0].uv = uv; DrawList._VtxWritePtr[0].col = col;
        DrawList._VtxWritePtr[1].pos.x = p2.x + dy; DrawList._VtxWritePtr[1].pos.y = p2.y - dx; DrawList._VtxWritePtr[1].uv = uv; DrawList._VtxWritePtr[1].col = col;
        DrawList._VtxWritePtr[2].pos.x = p2.x - dy; DrawList._VtxWritePtr[2].pos.y = p2.y + dx; DrawList._VtxWritePtr[2].uv = uv; DrawList._VtxWritePtr[2].col = col;
        DrawList._VtxWritePtr[3].pos.x = p1.x - dy; DrawList._VtxWritePtr[3].pos.y = p1.y + dx; DrawList._VtxWritePtr[3].uv = uv; DrawList._VtxWritePtr[3].col = col;
        DrawList._VtxWritePtr += 4;

        DrawList._IdxWritePtr[0] = (ImDrawIdx)(DrawList._VtxCurrentIdx); DrawList._IdxWritePtr[1] = (ImDrawIdx)(DrawList._VtxCurrentIdx+1); DrawList._IdxWritePtr[2] = (ImDrawIdx)(DrawList._VtxCurrentIdx+2);
        DrawList._IdxWritePtr[3] = (ImDrawIdx)(DrawList._VtxCurrentIdx); DrawList._IdxWritePtr[4] = (ImDrawIdx)(DrawList._VtxCurrentIdx+2); DrawList._IdxWritePtr[5] = (ImDrawIdx)(DrawList._VtxCurrentIdx+3);
        DrawList._IdxWritePtr += 6;
        DrawList._VtxCurrentIdx += 4;
    }
}

inline void RenderPlotItemScatter(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.x_axis.maximum - plot.x_axis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.y_axis.maximum - plot.y_axis.minimum);
    for (std::size_t i = 0; i < item.data.size(); ++i)
    {
        ImVec2 c;
        c.x = pix.Min.x + mx * (item.data[i].x - plot.x_axis.minimum);
        c.y = pix.Min.y + my * (item.data[i].y - plot.y_axis.minimum);
        DrawList.AddCircleFilled(c, item.size, col, 10);
    }
}

inline void RenderPlotItemXBar(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.x_axis.maximum - plot.x_axis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.y_axis.maximum - plot.y_axis.minimum);
    const float halfSize = 0.5f * item.size;
    for (std::size_t i = 0; i < item.data.size(); ++i)
    {
        if (item.data[i].y == 0)
            continue;
        float y1 = pix.Min.y + my * (item.data[i].y - plot.y_axis.minimum);
        float y2 = pix.Min.y + my * (-plot.y_axis.minimum);
        float l = pix.Min.x + mx * (item.data[i].x - halfSize - plot.x_axis.minimum);
        float r = pix.Min.x + mx * (item.data[i].x + halfSize - plot.x_axis.minimum);
        DrawList.AddRectFilled({l, ImMin(y1, y2)}, {r, ImMax(y1, y2)}, col);
    }
}

inline void RenderPlotItemYBar(const PlotItem &item, const PlotInterface &plot, const ImRect &pix, ImDrawList &DrawList)
{
    const ImU32 col = GetColorU32(item.color);
    const float mx = (pix.Max.x - pix.Min.x) / (plot.x_axis.maximum - plot.x_axis.minimum);
    const float my = (pix.Max.y - pix.Min.y) / (plot.y_axis.maximum - plot.y_axis.minimum);
    const float halfSize = 0.5f * item.size;
    for (std::size_t i = 0; i < item.data.size(); ++i)
    {
        if (item.data[i].x == 0)
            continue;
        float x1 = pix.Min.x + mx * (item.data[i].x - plot.x_axis.minimum);
        float x2 = pix.Min.x + mx * (0 - plot.x_axis.minimum);
        float t = pix.Min.y + my * (item.data[i].y + halfSize - plot.y_axis.minimum);
        float b = pix.Min.y + my * (item.data[i].y - halfSize - plot.y_axis.minimum);
        DrawList.AddRectFilled({ImMin(x1, x2), t}, {ImMax(x1, x2), b}, col);
    }
}

} // namespace

PlotItem::PlotItem() : show(true), type(PlotItem::Line), data(), size(1), data_begin(0)
{
    static std::vector<ImVec4> default_colors = {
        {(0.0F), (0.7490196228F), (1.0F), (1.0F)},                    // Blues::DeepSkyBlue,
        {(1.0F), (0.0F), (0.0F), (1.0F)},                             // Reds::Red,
        {(0.4980392158F), (1.0F), (0.0F), (1.0F)},                    // Greens::Chartreuse,
        {(1.0F), (1.0F), (0.0F), (1.0F)},                             // Yellows::Yellow,
        {(0.0F), (1.0F), (1.0F), (1.0F)},                             // Cyans::Cyan,
        {(1.0F), (0.6470588446F), (0.0F), (1.0F)},                    // Oranges::Orange,
        {(1.0F), (0.0F), (1.0F), (1.0F)},                             // Purples::Magenta,
        {(0.5411764979F), (0.1686274558F), (0.8862745166F), (1.0F)},  // Purples::BlueViolet,
        {(0.8784313798F), (1.0F), (1.0F), (1.0F)},                    // Cyans::LightCyan,
        {(0.8235294223F), (0.7058823705F), (0.5490196347F), (1.0F)}   // Browns::Tan
    };
    static int nextColor = 0;
    static int itemNumber = 0;
    color = default_colors[nextColor];
    nextColor = (nextColor + 1) % default_colors.size();
    label = "item" + std::to_string(itemNumber++);
}

PlotAxis::PlotAxis() : show_grid(true), show_tick_marks(true), show_tick_labels(true), minimum(0), maximum(1), divisions(3), subdivisions(10),
                       adaptive(true), color(0,0,0,-1), zoom_rate(0.1f), lock_min(false), lock_max(false), flip(false), label("")
{
}

PlotInterface::PlotInterface() : show_crosshairs(false), show_mouse_pos(true), show_legend(true), enable_selection(true), enable_controls(true), title(""),
                                 _dragging_x(false), _dragging_y(false), _selecting(false)
{
    frame_color = ImVec4(0,0,0,-1);
    background_color = ImVec4(0,0,0,-1);
    border_color = ImVec4(0,0,0,-1);
    selection_color = {.118f, .565f, 1, 0.25};
}

void Plot(const char *label_id, PlotInterface &plot, std::vector<PlotItem> &items, const ImVec2 &size)
{
    Plot(label_id, &plot, &items[0], (int)items.size(), size);
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
    const ImU32 color_frame = plot.frame_color.w == -1 ? GetColorU32(ImGuiCol_FrameBg) : GetColorU32(plot.frame_color);
    const ImU32 color_bg = plot.background_color.w == -1 ? GetColorU32(ImGuiCol_WindowBg) : GetColorU32(plot.background_color);
    const ImU32 color_border = plot.border_color.w == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(plot.border_color);

    const ImVec4 color_xAxis = plot.x_axis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.x_axis.color;
    const ImU32 color_x1 = GetColorU32(color_xAxis);
    const ImU32 color_x2 = GetColorU32(color_xAxis * ImVec4(1, 1, 1, 0.25f));
    const ImU32 color_xtxt = GetColorU32({color_xAxis.x, color_xAxis.y, color_xAxis.z, 1});

    const ImVec4 color_yAxis = plot.y_axis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.y_axis.color;
    const ImU32 color_y1 = GetColorU32(color_yAxis);
    const ImU32 color_y2 = GetColorU32(color_yAxis * ImVec4(1, 1, 1, 0.25f));
    const ImU32 color_ytxt = GetColorU32({color_yAxis.x, color_yAxis.y, color_yAxis.z, 1});

    const ImU32 color_txt = GetColorU32(ImGuiCol_Text);
    const ImU32 color_txt_dis = GetColorU32(ImGuiCol_TextDisabled);
    const ImU32 color_slctBg = GetColorU32(plot.selection_color);
    const ImU32 color_slctBd = GetColorU32({plot.selection_color.x, plot.selection_color.y, plot.selection_color.z, 1});

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

    // constrain axes
    if (plot.x_axis.maximum <= plot.x_axis.minimum)
        plot.x_axis.maximum = plot.x_axis.minimum + std::numeric_limits<float>::epsilon();
    if (plot.y_axis.maximum <= plot.y_axis.minimum)
        plot.y_axis.maximum = plot.y_axis.minimum + std::numeric_limits<float>::epsilon();

    // adaptive divisions
    if (plot.x_axis.adaptive) {
        plot.x_axis.divisions = (int)std::round(0.003 * canvas_bb.GetWidth());
        if (plot.x_axis.divisions < 2)
            plot.x_axis.divisions = 2;
    }
    if (plot.y_axis.adaptive) {
        plot.y_axis.divisions = (int)std::round(0.003 * canvas_bb.GetHeight());
        if (plot.y_axis.divisions < 2)
            plot.y_axis.divisions = 2;
    }

    // get ticks
    static std::vector<Tick> xTicks(100), yTicks(100);
    const bool renderX = (plot.x_axis.show_grid || plot.x_axis.show_tick_marks || plot.x_axis.show_tick_labels) && plot.x_axis.divisions > 1;
    const bool renderY = (plot.y_axis.show_grid || plot.y_axis.show_tick_marks || plot.y_axis.show_tick_labels) && plot.y_axis.divisions > 1;

    if (renderX)
        GetTicks(plot.x_axis.minimum, plot.x_axis.maximum, plot.x_axis.divisions, plot.x_axis.subdivisions, xTicks);
    if (renderY)
        GetTicks(plot.y_axis.minimum, plot.y_axis.maximum, plot.y_axis.divisions, plot.y_axis.subdivisions, yTicks);

    // label ticks
    if (plot.x_axis.show_tick_labels)
        LabelTicks(xTicks);
    if (plot.y_axis.show_tick_labels)
        LabelTicks(yTicks);



    // get max y-tick width
    float maxLabelWidth = 0;
    if (plot.y_axis.show_tick_labels)
    {
        for (auto &yt : yTicks)
            maxLabelWidth = yt.size.x > maxLabelWidth ? yt.size.x : maxLabelWidth;
    }

    // grid bb
    const float textOffset = 5;
    const float textHeight = GetTextLineHeight();
    const float tPadding = plot.title != "" ? textHeight + textOffset : 0;
    const float bPadding = (plot.x_axis.show_tick_labels ? textHeight + textOffset : 0) + (plot.x_axis.label != "" ? textHeight + textOffset : 0);
    const float lPadding = (plot.y_axis.show_tick_labels ? maxLabelWidth + textOffset : 0) + (plot.y_axis.label != "" ? textHeight + textOffset : 0);
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
    if (plot.show_legend && nItems > 0)
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

    g_plot_area_hovered = frame_hovered && grid_hovered && !legend_hovered;

    // end drags
    if (plot._dragging_x && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        plot._dragging_x = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0;
    }
    if (plot._dragging_y && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        plot._dragging_y = false;   
        G.IO.MouseDragMaxDistanceSqr[0] = 0; 
    }
    // do drag
    if (plot._dragging_x || plot._dragging_y)
    {
        bool xLocked = plot.x_axis.lock_min && plot.x_axis.lock_max;
        if (!xLocked && plot._dragging_x)
        {
            float dir = plot.x_axis.flip ? -1.0f : 1.0f;
            float delX = dir * IO.MouseDelta.x * (plot.x_axis.maximum - plot.x_axis.minimum) / (grid_bb.Max.x - grid_bb.Min.x);
            if (!plot.x_axis.lock_min)
                plot.x_axis.minimum -= delX;
            if (!plot.x_axis.lock_max)
                plot.x_axis.maximum -= delX;
        }
        bool yLocked = plot.y_axis.lock_min && plot.y_axis.lock_max;
        if (!yLocked && plot._dragging_y)
        {
            float dir = plot.y_axis.flip ? -1.0f : 1.0f;
            float delY = dir * IO.MouseDelta.y * (plot.y_axis.maximum - plot.y_axis.minimum) / (grid_bb.Max.y - grid_bb.Min.y);
            if (!plot.y_axis.lock_min)
                plot.y_axis.minimum += delY;
            if (!plot.y_axis.lock_max)
                plot.y_axis.maximum += delY;
        }
        if ((xLocked && yLocked) || (xLocked && plot._dragging_x && !plot._dragging_y) || (yLocked && plot._dragging_y && !plot._dragging_x))
            ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
        else if (xLocked || (!plot._dragging_x && plot._dragging_y))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        else if (yLocked || (!plot._dragging_y && plot._dragging_x))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    // start drag
    if (frame_hovered && xAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !plot._selecting &&!legend_hovered)
        plot._dragging_x = true;
    if (frame_hovered && yAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !plot._selecting &&!legend_hovered)
        plot._dragging_y = true;
    
    // scroll zoom
    if (frame_hovered && (xAxisRegion_hovered || yAxisRegion_hovered))
    {
        float xRange = plot.x_axis.maximum - plot.x_axis.minimum;
        float yRange = plot.y_axis.maximum - plot.y_axis.minimum;
        if (IO.MouseWheel < 0)
        {
            if (xAxisRegion_hovered) {
                if (!plot.x_axis.lock_min)
                    plot.x_axis.minimum -= plot.x_axis.zoom_rate * xRange;
                if (!plot.x_axis.lock_max)
                    plot.x_axis.maximum += plot.x_axis.zoom_rate * xRange;
            }
            if (yAxisRegion_hovered) {
                if (!plot.y_axis.lock_min)
                    plot.y_axis.minimum -= plot.y_axis.zoom_rate * yRange;
                if (!plot.y_axis.lock_max)
                    plot.y_axis.maximum += plot.y_axis.zoom_rate * yRange;
            }
        }
        if (IO.MouseWheel > 0)
        {
            if (xAxisRegion_hovered) {
                if (!plot.x_axis.lock_min)
                    plot.x_axis.minimum += plot.x_axis.zoom_rate * xRange / (1.0f + 2.0f * plot.x_axis.zoom_rate);
                if (!plot.x_axis.lock_max)
                    plot.x_axis.maximum -= plot.x_axis.zoom_rate * xRange / (1.0f + 2.0f * plot.x_axis.zoom_rate);
            }
            if (yAxisRegion_hovered) {
                if (!plot.y_axis.lock_min)
                    plot.y_axis.minimum += plot.y_axis.zoom_rate * yRange / (1.0f + 2.0f * plot.y_axis.zoom_rate);
                if (!plot.y_axis.lock_max)
                    plot.y_axis.maximum -= plot.y_axis.zoom_rate * yRange / (1.0f + 2.0f * plot.y_axis.zoom_rate);
            }
        }
    }

    // get pixels for transforms
    const ImRect pix(plot.x_axis.flip ? grid_bb.Max.x : grid_bb.Min.x,
                     plot.y_axis.flip ? grid_bb.Min.y : grid_bb.Max.y,
                     plot.x_axis.flip ? grid_bb.Min.x : grid_bb.Max.x,
                     plot.y_axis.flip ? grid_bb.Max.y : grid_bb.Min.y);

    // set mouse position
    g_plot_mouse_pos.x = Remap(IO.MousePos.x, pix.Min.x, pix.Max.x, plot.x_axis.minimum, plot.x_axis.maximum);
    g_plot_mouse_pos.y = Remap(IO.MousePos.y, pix.Min.y, pix.Max.y, plot.y_axis.minimum, plot.y_axis.maximum);

    // confirm selection
    if (plot._selecting && (IO.MouseReleased[1] || !IO.MouseDown[1]))
    {
        ImVec2 slcSize = plot._select_start - IO.MousePos;
        if (std::abs(slcSize.x) > 2 && std::abs(slcSize.y) > 2)
        {
            ImVec2 p1, p2;
            p1.x = Remap(plot._select_start.x, pix.Min.x, pix.Max.x, plot.x_axis.minimum, plot.x_axis.maximum);
            p1.y = Remap(plot._select_start.y, pix.Min.y, pix.Max.y, plot.y_axis.minimum, plot.y_axis.maximum);
            p2.x = Remap(IO.MousePos.x, pix.Min.x, pix.Max.x, plot.x_axis.minimum, plot.x_axis.maximum);
            p2.y = Remap(IO.MousePos.y, pix.Min.y, pix.Max.y, plot.y_axis.minimum, plot.y_axis.maximum);
            plot.x_axis.minimum = ImMin(p1.x, p2.x);
            plot.x_axis.maximum = ImMax(p1.x, p2.x);
            plot.y_axis.minimum = ImMin(p1.y, p2.y);
            plot.y_axis.maximum = ImMax(p1.y, p2.y);
        }
        plot._selecting = false;
    }
    // cancel selection
    if (plot._selecting && (IO.MouseClicked[0] || IO.MouseDown[0]))
    {
        plot._selecting = false;
    }
    // begin selection
    if (frame_hovered && grid_hovered && IO.MouseClicked[1] && plot.enable_selection)
    {
        plot._select_start = IO.MousePos;
        plot._selecting = true;
    }

    // RENDER

    // grid bg
    DrawList.AddRectFilled(grid_bb.Min, grid_bb.Max, color_bg);

    // render axes
    DrawList.PushClipRect(grid_bb.Min, grid_bb.Max);

    // transform ticks
    if (renderX)
        TransformTicks(xTicks, plot.x_axis.minimum, plot.x_axis.maximum, pix.Min.x, pix.Max.x);
    if (renderY)
        TransformTicks(yTicks, plot.y_axis.minimum, plot.y_axis.maximum, pix.Min.y, pix.Max.y);

    // render grid
    if (plot.x_axis.show_grid)
    {
        for (auto &xt : xTicks)
            DrawList.AddLine({xt.pixels, grid_bb.Min.y}, {xt.pixels, grid_bb.Max.y}, xt.major ? color_x1 : color_x2, 1);
    }

    if (plot.y_axis.show_grid)
    {
        for (auto &yt : yTicks)
            DrawList.AddLine({grid_bb.Min.x, yt.pixels}, {grid_bb.Max.x, yt.pixels}, yt.major ? color_y1 : color_y2, 1);
    }

    // render plot items
    for (int i = 0; i < nItems; ++i) {
        if (items[i].show && items[i].data.size() > 0) {
            if (items[i].type == PlotItem::Line) 
#ifdef IMGUI_PLOT_LINE_USE_AA
                RenderPlotItemLineAA(items[i], plot, pix, DrawList);
#else
                RenderPlotItemLine(items[i], plot, pix, DrawList);
#endif
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
        ImRect select_bb(ImMin(IO.MousePos, plot._select_start), ImMax(IO.MousePos, plot._select_start));
        if (select_bb.GetWidth() > 2 && select_bb.GetHeight() > 2) {
            DrawList.AddRectFilled(select_bb.Min, select_bb.Max, color_slctBg);
            DrawList.AddRect(select_bb.Min, select_bb.Max, color_slctBd);
        }
    }

    // render ticks
    if (plot.x_axis.show_tick_marks)
    {
        for (auto &xt : xTicks)
            DrawList.AddLine({xt.pixels, grid_bb.Max.y}, {xt.pixels, grid_bb.Max.y - (xt.major ? 10.0f : 5.0f)}, color_border, 1);
    }
    if (plot.y_axis.show_tick_marks)
    {
        for (auto &yt : yTicks)
            DrawList.AddLine({grid_bb.Min.x, yt.pixels}, {grid_bb.Min.x + (yt.major ? 10.0f : 5.0f), yt.pixels}, color_border, 1);
    }

    // render crosshairs
    if (plot.show_crosshairs && grid_hovered && frame_hovered && !(plot._dragging_x || plot._dragging_y) && !plot._selecting && !legend_hovered)
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
    if (plot.show_legend && nItems > 0)
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
    if (plot.show_mouse_pos && grid_hovered)
    {
        static char buffer[32];
        sprintf(buffer, "%.2f,%.2f", g_plot_mouse_pos.x, g_plot_mouse_pos.y);
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
    if (plot.x_axis.show_tick_labels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &xt : xTicks)
            DrawList.AddText({xt.pixels - xt.size.x * 0.5f, grid_bb.Max.y + textOffset}, color_xtxt, xt.txt.c_str());
        DrawList.PopClipRect();
    }
    if (plot.x_axis.label != "") {
        const ImVec2 xLabel_size = CalcTextSize(plot.x_axis.label.c_str());
        const ImVec2 xLabel_pos(grid_bb.GetCenter().x - xLabel_size.x * 0.5f, canvas_bb.Max.y - textHeight);
        DrawList.AddText(xLabel_pos, color_xtxt, plot.x_axis.label.c_str());
    }
    if (plot.y_axis.show_tick_labels)
    {
        DrawList.PushClipRect(frame_bb.Min, frame_bb.Max);
        for (auto &yt : yTicks)
            DrawList.AddText({grid_bb.Min.x - textOffset - yt.size.x, yt.pixels - 0.5f * yt.size.y}, color_ytxt, yt.txt.c_str());
        DrawList.PopClipRect();
    }
    if (plot.y_axis.label != "") {
        const ImVec2 yLabel_size = CalcTextSizeVertical(plot.y_axis.label.c_str());
        const ImVec2 yLabel_pos(canvas_bb.Min.x, grid_bb.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, plot.y_axis.label.c_str(), yLabel_pos, color_ytxt);
    }

    // double click
    if (frame_hovered && grid_hovered && IO.MouseDoubleClicked[0] && nItems > 0) {
        float new_x_min = INFINITY;
        float new_x_max = -INFINITY;
        float new_y_min = INFINITY;
        float new_y_max = -INFINITY;
        bool fit = false;
        for (int i = 0; i < nItems; ++i) {
            if (items[i].show) {
                for (auto& d : items[i].data) {
                    new_x_min = d.x < new_x_min ? d.x : new_x_min;
                    new_x_max = d.x > new_x_max ? d.x : new_x_max;
                    new_y_min = d.y < new_y_min ? d.y : new_y_min;
                    new_y_max = d.y > new_y_max ? d.y : new_y_max;
                }
                fit = true;
            }
        }
        if (fit) {
            if (!plot.x_axis.lock_min)
                plot.x_axis.minimum = new_x_min;
            if (!plot.x_axis.lock_max)
                plot.x_axis.maximum = new_x_max;
            if (!plot.y_axis.lock_min)
                plot.y_axis.minimum = new_y_min;
            if (!plot.y_axis.lock_max)
                plot.y_axis.maximum = new_y_max;
        }
    }

    // AddTextVertical(&DrawList, "Hello, World", frame_bb.Min, color_ytxt);

    PushID(id);
    if (frame_hovered && grid_hovered && IO.MouseDoubleClicked[1] && plot.enable_controls && !legend_hovered)
        ImGui::OpenPopup("##Context");
    if (ImGui::BeginPopup("##Context"))
    {
        ImGui::PushItemWidth(75);

        if (plot.x_axis.lock_min) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f); }
        ImGui::DragFloat("##Xmin", &plot.x_axis.minimum, 0.01f + 0.01f * (plot.x_axis.maximum - plot.x_axis.minimum), -INFINITY, plot.x_axis.maximum - FLT_EPSILON);
        if (plot.x_axis.lock_min) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }
        ImGui::SameLine();
        ImGui::Checkbox("##LockXMin",&plot.x_axis.lock_min);
        ImGui::SameLine();

        if (plot.x_axis.lock_max) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f); }
        ImGui::DragFloat("##Xmax", &plot.x_axis.maximum, 0.01f + 0.01f * (plot.x_axis.maximum - plot.x_axis.minimum), plot.x_axis.minimum + FLT_EPSILON, INFINITY);
        if (plot.x_axis.lock_max) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }
        ImGui::SameLine();
        ImGui::Checkbox("X-Axis",&plot.x_axis.lock_max);
        ImGui::Checkbox("Grid##X", &plot.x_axis.show_grid);
        ImGui::SameLine();
        ImGui::Checkbox("Flip##X", &plot.x_axis.flip);

        ImGui::Separator();

        if (plot.y_axis.lock_min) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f); }
        ImGui::DragFloat("##Ymin", &plot.y_axis.minimum, 0.01f + 0.01f * (plot.y_axis.maximum - plot.y_axis.minimum), -INFINITY, plot.y_axis.maximum - FLT_EPSILON);
        if (plot.y_axis.lock_min) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }
        ImGui::SameLine();
        ImGui::Checkbox("##LockYMin",&plot.y_axis.lock_min);
        ImGui::SameLine();

        if (plot.y_axis.lock_max) { ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f); }
        ImGui::DragFloat("##Ymax", &plot.y_axis.maximum, 0.01f + 0.01f * (plot.y_axis.maximum - plot.y_axis.minimum), plot.y_axis.minimum + FLT_EPSILON, INFINITY);
        if (plot.y_axis.lock_max) { ImGui::PopItemFlag(); ImGui::PopStyleVar(); }
        ImGui::SameLine();
        ImGui::Checkbox("Y-Axis",&plot.y_axis.lock_max);
        ImGui::Checkbox("Grid##Y", &plot.y_axis.show_grid);
        ImGui::SameLine();
        ImGui::Checkbox("Flip##Y", &plot.y_axis.flip);

        ImGui::PopItemWidth();  

        ImGui::Separator();
        ImGui::Checkbox("Legend", &plot.show_legend);
        ImGui::SameLine();
        ImGui::Checkbox("Crosshairs", &plot.show_crosshairs);

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


void PlotItemRollPoint(PlotItem& item, double x, double y, double span) {
    double xmod = ImFmod(x, span);
    if (!item.data.empty() && xmod < item.data.back().x)
        item.data.clear();
    item.data.push_back(ImVec2(static_cast<float>(xmod), static_cast<float>(y)));
}

void PlotItemBufferPoint(PlotItem& item, double x, double y, int max_points) {
    if (item.data.size() < static_cast<std::size_t>(max_points)) 
        item.data.push_back(ImVec2(static_cast<float>(x),static_cast<float>(y)));
    else {
        item.data[item.data_begin] = ImVec2(static_cast<float>(x),static_cast<float>(y));
        item.data_begin++;
        if (item.data_begin == max_points)
            item.data_begin = 0;
    }
}

void PlotAxisScroll(PlotAxis& axis, double current_time, double history) {
    axis.maximum = static_cast<float>(current_time);
    axis.minimum = static_cast<float>(current_time - history);
}


bool IsPlotHovered() {
    return g_plot_area_hovered;
}

ImVec2 GetPlotMousePos() {
    return g_plot_mouse_pos;
}

}