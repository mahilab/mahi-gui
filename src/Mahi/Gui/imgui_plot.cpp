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

#define IM_NORMALIZE2F_OVER_ZERO(VX, VY)                                                           \
    {                                                                                              \
        float d2 = VX * VX + VY * VY;                                                              \
        if (d2 > 0.0f) {                                                                           \
            float inv_len = 1.0f / ImSqrt(d2);                                                     \
            VX *= inv_len;                                                                         \
            VY *= inv_len;                                                                         \
        }                                                                                          \
    }

namespace ImGui {

namespace {

/// Linearly remaps float x from [x0 x1] to [y0 y1].
inline float Remap(float x, float x0, float x1, float y0, float y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

/// Utility function to that rounds x to powers of 2,5 and 10 for generating axis labels
/// Taken from Graphics Gems 1 Chapter 11.2, "Nice Numbers for Graph Labels"
inline double NiceNum(double x, bool round) {
    double f;  /* fractional part of x */
    double nf; /* nice, rounded fraction */
    int    expv = (int)floor(log10(x));
    f           = x / std::pow(10, expv); /* between 1 and 10 */
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
inline void AddTextVertical(ImDrawList *DrawList, const char *text, ImVec2 pos, ImU32 text_color) {
    pos.x                   = IM_ROUND(pos.x);
    pos.y                   = IM_ROUND(pos.y);
    ImFont *           font = GImGui->Font;
    const ImFontGlyph *glyph;
    char               c;
    while ((c = *text++)) {
        glyph = font->FindGlyph(c);
        if (!glyph)
            continue;

        DrawList->PrimReserve(6, 4);
        DrawList->PrimQuadUV(
            pos + ImVec2(glyph->Y0, -glyph->X0), pos + ImVec2(glyph->Y0, -glyph->X1),
            pos + ImVec2(glyph->Y1, -glyph->X1), pos + ImVec2(glyph->Y1, -glyph->X0),

            ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V0),
            ImVec2(glyph->U1, glyph->V1), ImVec2(glyph->U0, glyph->V1), text_color);
        pos.y -= glyph->AdvanceX;
    }
}

/// Calculates the size of vertical text
inline ImVec2 CalcTextSizeVertical(const char *text) {
    ImVec2 sz = CalcTextSize(text);
    return ImVec2(sz.y, sz.x);
}

/// Tick mark info
struct ImTick {
    double      plot;
    float       pixels;
    bool        major;
    std::string txt;
    ImVec2      size;
};

inline void GetTicks(float tMin, float tMax, int nMajor, int nMinor, std::vector<ImTick> &out) {
    // out.shrink(0);
    out.clear();
    const double range    = NiceNum(tMax - tMin, 0);
    const double interval = NiceNum(range / (nMajor - 1), 1);
    const double graphmin = floor(tMin / interval) * interval;
    const double graphmax = ceil(tMax / interval) * interval;
    // const int nfrac = std::max((int)-floor(log10(interval)), 0);
    for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval) {
        if (major >= tMin && major <= tMax)
            out.push_back({major, 0, true});
        for (int i = 1; i < nMinor; ++i) {
            double minor = major + i * interval / nMinor;
            if (minor >= tMin && minor <= tMax)
                out.push_back({minor, 0, false});
        }
    }
}

inline void LabelTicks(std::vector<ImTick> &ticks) {
    for (auto &tk : ticks) {
        std::stringstream ss;
        ss << tk.plot;
        tk.txt  = ss.str();
        tk.size = CalcTextSize(tk.txt.c_str());
    }
}

inline void TransformTicks(std::vector<ImTick> &ticks, float tMin, float tMax, float pMin,
                           float pMax) {
    const float m = (pMax - pMin) / (tMax - tMin);
    for (auto &tk : ticks)
        tk.pixels = (pMin + m * ((float)tk.plot - tMin));  
}

/// Holds Plot state information in between calls to BeginPlot()/EndPlot()
struct ImPlot {
    ImPlot() {
        active = false;
        x_ticks.reserve(50);
        y_ticks.reserve(50);
    }
    /// Current Plot
    PlotInterface* plot;
    /// True if we are between calls to BeginPlot()/EndPlot()
    bool active;
    // Bounding regions
    ImRect bb_frame;
    ImRect bb_canvas;
    ImRect bb_grid;
    // Hover states
    bool hov_frame;
    bool hov_grid;

    // Colors
    ImU32 col_frame, col_bg, col_border, 
          col_txt, col_txt_dis, 
          col_slct_bg, col_slct_bd,
          col_x1, col_x2, col_x_txt,
          col_y1, col_y2, col_y_txt;     
    // Tick marks     
    std::vector<ImTick> x_ticks, y_ticks;
    // Render flags
    bool rend_x, rend_y;
    // Mouse pos
    ImVec2 mouse_pos;
};

/// Current Plot state information
static ImPlot g_plt;

} // private namespace

bool BeginPlot(const char* label_id, PlotInterface* plot_ptr, const ImVec2& size, ImGuiPlotFlags flags) {
    IM_ASSERT_USER_ERROR(g_plt.active == false, "Mismatched BeginPlot()/EndPlot()!");

    g_plt.plot = plot_ptr;
    PlotInterface &plot = *plot_ptr;

    // ImGui front matter
    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = GetIO();
    ImDrawList &      DrawList = *Window->DrawList;
    const ImGuiID     id       = Window->GetID(label_id);

    // get colors
    g_plt.col_frame = plot.frame_color.w == -1 ? GetColorU32(ImGuiCol_FrameBg) : GetColorU32(plot.frame_color);
    g_plt.col_bg = plot.background_color.w == -1 ? GetColorU32(ImGuiCol_WindowBg) : GetColorU32(plot.background_color);
    g_plt.col_border = plot.border_color.w == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(plot.border_color);

    const ImVec4 col_xAxis = plot.x_axis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.x_axis.color;
    g_plt.col_x1   = GetColorU32(col_xAxis);
    g_plt.col_x2   = GetColorU32(col_xAxis * ImVec4(1, 1, 1, 0.25f));
    g_plt.col_x_txt = GetColorU32({col_xAxis.x, col_xAxis.y, col_xAxis.z, 1});

    const ImVec4 col_yAxis = plot.y_axis.color.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.y_axis.color;
    g_plt.col_y1   = GetColorU32(col_yAxis);
    g_plt.col_y2   = GetColorU32(col_yAxis * ImVec4(1, 1, 1, 0.25f));
    g_plt.col_y_txt = GetColorU32({col_yAxis.x, col_yAxis.y, col_yAxis.z, 1});

    g_plt.col_txt     = GetColorU32(ImGuiCol_Text);
    g_plt.col_txt_dis = GetColorU32(ImGuiCol_TextDisabled);
    g_plt.col_slct_bg  = GetColorU32(plot.selection_color);
    g_plt.col_slct_bd =  GetColorU32({plot.selection_color.x, plot.selection_color.y, plot.selection_color.z, 1});

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    g_plt.bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(g_plt.bb_frame);
    if (!ItemAdd(g_plt.bb_frame, 0, &g_plt.bb_frame))
        return false;
    g_plt.hov_frame = ItemHoverable(g_plt.bb_frame, id);
    RenderFrame(g_plt.bb_frame.Min, g_plt.bb_frame.Max, g_plt.col_frame, true, Style.FrameRounding);

    // canvas bb
    g_plt.bb_canvas = ImRect(g_plt.bb_frame.Min + Style.WindowPadding, g_plt.bb_frame.Max - Style.WindowPadding);

    // constrain axes
    if (plot.x_axis.maximum <= plot.x_axis.minimum)
        plot.x_axis.maximum = plot.x_axis.minimum + FLT_EPSILON;
    if (plot.y_axis.maximum <= plot.y_axis.minimum)
        plot.y_axis.maximum = plot.y_axis.minimum + FLT_EPSILON;

    // adaptive divisions
    if (plot.x_axis.adaptive) {
        plot.x_axis.divisions = (int)std::round(0.003 * g_plt.bb_canvas.GetWidth());
        if (plot.x_axis.divisions < 2)
            plot.x_axis.divisions = 2;
    }
    if (plot.y_axis.adaptive) {
        plot.y_axis.divisions = (int)std::round(0.003 * g_plt.bb_canvas.GetHeight());
        if (plot.y_axis.divisions < 2)
            plot.y_axis.divisions = 2;
    }

    g_plt.rend_x = (plot.x_axis.show_grid || plot.x_axis.show_tick_marks || plot.x_axis.show_tick_labels) &&  plot.x_axis.divisions > 1;
    g_plt.rend_y = (plot.y_axis.show_grid || plot.y_axis.show_tick_marks || plot.y_axis.show_tick_labels) &&  plot.y_axis.divisions > 1;

    // get ticks
    if (g_plt.rend_x)
        GetTicks(plot.x_axis.minimum, plot.x_axis.maximum, plot.x_axis.divisions, plot.x_axis.subdivisions, g_plt.x_ticks);
    if (g_plt.rend_y)
        GetTicks(plot.y_axis.minimum, plot.y_axis.maximum, plot.y_axis.divisions, plot.y_axis.subdivisions, g_plt.y_ticks);

    // label ticks
    if (plot.x_axis.show_tick_labels)
        LabelTicks(g_plt.x_ticks);
    if (plot.y_axis.show_tick_labels)
        LabelTicks(g_plt.y_ticks);

    // get max y-tick width
    float maxLabelWidth = 0;
    if (plot.y_axis.show_tick_labels) {
        for (auto &yt : g_plt.y_ticks)
            maxLabelWidth = yt.size.x > maxLabelWidth ? yt.size.x : maxLabelWidth;
    }

    // grid bb
    const float txt_off    = 5;
    const float txt_height = GetTextLineHeight();
    const float pad_top    = plot.title != "" ? txt_height + txt_off : 0;
    const float pad_bot    = (plot.x_axis.show_tick_labels ? txt_height + txt_off : 0) + (plot.x_axis.label != "" ? txt_height + txt_off : 0);
    const float pad_left   = (plot.y_axis.show_tick_labels ? maxLabelWidth + txt_off : 0) + (plot.y_axis.label != "" ? txt_height + txt_off : 0);
    g_plt.bb_grid          = ImRect(g_plt.bb_canvas.Min + ImVec2(pad_left, pad_top), g_plt.bb_canvas.Max - ImVec2(0, pad_bot));
    g_plt.hov_grid         = g_plt.bb_grid.Contains(IO.MousePos);

    // axis region bbs
    const ImRect xAxisRegion_bb(g_plt.bb_grid.Min + ImVec2(10, 0), {g_plt.bb_grid.Max.x, g_plt.bb_frame.Max.y});
    const bool   xAxisRegion_hovered = xAxisRegion_bb.Contains(IO.MousePos);
    const ImRect yAxisRegion_bb({g_plt.bb_frame.Min.x, g_plt.bb_grid.Min.y}, g_plt.bb_grid.Max - ImVec2(0, 10));
    const bool   yAxisRegion_hovered = yAxisRegion_bb.Contains(IO.MousePos);

    // TODO

    // get pixels for transforms
    const ImRect pix(plot.x_axis.flip ? g_plt.bb_grid.Max.x : g_plt.bb_grid.Min.x,
                     plot.y_axis.flip ? g_plt.bb_grid.Min.y : g_plt.bb_grid.Max.y,
                     plot.x_axis.flip ? g_plt.bb_grid.Min.x : g_plt.bb_grid.Max.x,
                     plot.y_axis.flip ? g_plt.bb_grid.Max.y : g_plt.bb_grid.Min.y);
    
    // set mouse position
    g_plt.mouse_pos.x = Remap(IO.MousePos.x, pix.Min.x, pix.Max.x, plot.x_axis.minimum, plot.x_axis.maximum);
    g_plt.mouse_pos.y = Remap(IO.MousePos.y, pix.Min.y, pix.Max.y, plot.y_axis.minimum, plot.y_axis.maximum);


    // TODO

    // focus window
    if ((IO.MouseClicked[0] || IO.MouseClicked[1]) && g_plt.hov_frame)
        FocusWindow(GetCurrentWindow());
            
    // RENDER

    // grid bg
    DrawList.AddRectFilled(g_plt.bb_grid.Min, g_plt.bb_grid.Max, g_plt.col_bg);

    // render axes
    ImGui::PushClipRect(g_plt.bb_grid.Min, g_plt.bb_grid.Max, true);

    // transform ticks
    if (g_plt.rend_x)
        TransformTicks(g_plt.x_ticks, plot.x_axis.minimum, plot.x_axis.maximum, pix.Min.x, pix.Max.x);
    if (g_plt.rend_y)
        TransformTicks(g_plt.y_ticks, plot.y_axis.minimum, plot.y_axis.maximum, pix.Min.y, pix.Max.y);

    // render grid
    if (plot.x_axis.show_grid) {
        for (auto &xt : g_plt.x_ticks)
            DrawList.AddLine({xt.pixels, g_plt.bb_grid.Min.y}, {xt.pixels, g_plt.bb_grid.Max.y}, xt.major ? g_plt.col_x1 : g_plt.col_x2, 1);
    }

    if (plot.y_axis.show_grid) {
        for (auto &yt : g_plt.y_ticks)
            DrawList.AddLine({g_plt.bb_grid.Min.x, yt.pixels}, {g_plt.bb_grid.Max.x, yt.pixels}, yt.major ? g_plt.col_y1 : g_plt.col_y2, 1);
    }

    // render ticks
    if (plot.x_axis.show_tick_marks) {
        for (auto &xt : g_plt.x_ticks)
            DrawList.AddLine({xt.pixels, g_plt.bb_grid.Max.y},{xt.pixels, g_plt.bb_grid.Max.y - (xt.major ? 10.0f : 5.0f)}, g_plt.col_border, 1);
    }
    if (plot.y_axis.show_tick_marks) {
        for (auto &yt : g_plt.y_ticks)
            DrawList.AddLine({g_plt.bb_grid.Min.x, yt.pixels}, {g_plt.bb_grid.Min.x + (yt.major ? 10.0f : 5.0f), yt.pixels}, g_plt.col_border, 1);
    }

    ImGui::PopClipRect();

    // render title
    if (plot.title != "") {
        const ImVec2 title_size = CalcTextSize(plot.title.c_str());
        DrawList.AddText(ImVec2(g_plt.bb_canvas.GetCenter().x - title_size.x * 0.5f, g_plt.bb_canvas.Min.y), g_plt.col_txt, plot.title.c_str());
    }

    // render labels
    if (plot.x_axis.show_tick_labels) {
        ImGui::PushClipRect(g_plt.bb_frame.Min, g_plt.bb_frame.Max, true);
        for (auto &xt : g_plt.x_ticks)
            DrawList.AddText({xt.pixels - xt.size.x * 0.5f, g_plt.bb_grid.Max.y + txt_off}, g_plt.col_x_txt,
                             xt.txt.c_str());
        ImGui::PopClipRect();
    }
    if (plot.x_axis.label != "") {
        const ImVec2 xLabel_size = CalcTextSize(plot.x_axis.label.c_str());
        const ImVec2 xLabel_pos(g_plt.bb_grid.GetCenter().x - xLabel_size.x * 0.5f,
                                g_plt.bb_grid.Max.y - txt_height);
        DrawList.AddText(xLabel_pos, g_plt.col_x_txt, plot.x_axis.label.c_str());
    }
    if (plot.y_axis.show_tick_labels) {
        ImGui::PushClipRect(g_plt.bb_frame.Min, g_plt.bb_frame.Max, true);
        for (auto &yt : g_plt.y_ticks)
            DrawList.AddText({g_plt.bb_grid.Min.x - txt_off - yt.size.x, yt.pixels - 0.5f * yt.size.y},
                             g_plt.col_y_txt, yt.txt.c_str());
        ImGui::PopClipRect();
    }
    if (plot.y_axis.label != "") {
        const ImVec2 yLabel_size = CalcTextSizeVertical(plot.y_axis.label.c_str());
        const ImVec2 yLabel_pos(g_plt.bb_grid.Min.x, g_plt.bb_grid.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, plot.y_axis.label.c_str(), yLabel_pos, g_plt.col_y_txt);
    }


    g_plt.active = true;
    return true;
}

void EndPlot() {
    IM_ASSERT_USER_ERROR(g_plt.active == false, "Mismatched BeginPlot()/EndPlot()!");
    PlotInterface &plot = *g_plt.plot;

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = GetIO();
    ImDrawList &      DrawList = *Window->DrawList;

    ImGui::PushClipRect(g_plt.bb_grid.Min, g_plt.bb_grid.Max, true);


    // render selection


    // render mouse pos
    if (plot.show_mouse_pos && g_plt.hov_grid) {
        static char buffer[32];
        sprintf(buffer, "%.2f,%.2f", g_plt.mouse_pos.x, g_plt.mouse_pos.y);
        ImVec2 size = CalcTextSize(buffer);
        ImVec2 pos  = g_plt.bb_grid.Max - size - ImVec2(5, 5);
        DrawList.AddText(pos, g_plt.col_txt, buffer);
    }

    ImGui::PopClipRect();

    DrawList.AddRect(g_plt.bb_grid.Min, g_plt.bb_grid.Max, g_plt.col_border);


    g_plt.active = false;
}


}  // namespace ImGui
