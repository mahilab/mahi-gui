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

template <typename TFlag, typename TEnum>
inline bool Flagged(TFlag f, TEnum e) {
    return (f & e) == e;
}

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
struct ImPlotContext {
    ImPlotContext() {
        active = false;
        x_ticks.reserve(50);
        y_ticks.reserve(50);
    }
    /// Current Plot
    ImPlot* plot;
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
    // Transformation
    ImRect xform;
    float mx, my;
    ImRect cull_rect;
    // Render flags
    bool rend_x, rend_y;
    // Mouse pos
    ImVec2 mouse_pos;
};

/// Global plot context
static ImPlotContext GPlt;

ImU32 NextColor() {
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
    static int next_color = 0;
    ImU32 col  = GetColorU32(default_colors[next_color]);
    next_color = (next_color + 1) % default_colors.size();
    return col;
}

} // private namespace

bool BeginPlot(const char* title, ImPlot* plot_ptr, const char* x_label, const char* y_label, const ImVec2& size) {
    IM_ASSERT_USER_ERROR(GPlt.active == false, "Mismatched BeginPlot()/EndPlot()!");

    GPlt.plot = plot_ptr;
    ImPlot &plot = *plot_ptr;

    // ImGui front matter
    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = GetIO();
    ImDrawList &      DrawList = *Window->DrawList;
    const ImGuiID     id       = Window->GetID(title);

    // get colors
    // GPlt.col_frame = plot.frame_color.w == -1 ? GetColorU32(ImGuiCol_FrameBg) : GetColorU32(plot.frame_color);
    // GPlt.col_bg = plot.background_color.w == -1 ? GetColorU32(ImGuiCol_WindowBg) : GetColorU32(plot.background_color);
    // GPlt.col_border = plot.border_color.w == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(plot.border_color);

    GPlt.col_frame = GetColorU32(ImGuiCol_FrameBg);
    GPlt.col_bg = GetColorU32(ImGuiCol_WindowBg);
    GPlt.col_border = GetColorU32(ImGuiCol_Text, 0.5f);

    const ImVec4 col_xAxis = plot.XAxis.Col.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.XAxis.Col;
    GPlt.col_x1   = GetColorU32(col_xAxis);
    GPlt.col_x2   = GetColorU32(col_xAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.col_x_txt = GetColorU32({col_xAxis.x, col_xAxis.y, col_xAxis.z, 1});

    const ImVec4 col_yAxis = plot.YAxis.Col.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : plot.YAxis.Col;
    GPlt.col_y1   = GetColorU32(col_yAxis);
    GPlt.col_y2   = GetColorU32(col_yAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.col_y_txt = GetColorU32({col_yAxis.x, col_yAxis.y, col_yAxis.z, 1});

    GPlt.col_txt     = GetColorU32(ImGuiCol_Text);
    GPlt.col_txt_dis = GetColorU32(ImGuiCol_TextDisabled);
    GPlt.col_slct_bg  = GetColorU32({.118f, .565f, 1, 0.25f});
    GPlt.col_slct_bd =  GetColorU32({.118f, .565f, 1, 1.00f});

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    GPlt.bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(GPlt.bb_frame);
    if (!ItemAdd(GPlt.bb_frame, 0, &GPlt.bb_frame))
        return false;
    GPlt.hov_frame = ItemHoverable(GPlt.bb_frame, id);
    RenderFrame(GPlt.bb_frame.Min, GPlt.bb_frame.Max, GPlt.col_frame, true, Style.FrameRounding);

    // canvas bb
    GPlt.bb_canvas = ImRect(GPlt.bb_frame.Min + Style.WindowPadding, GPlt.bb_frame.Max - Style.WindowPadding);

    // constrain axes
    if (plot.XAxis.Max <= plot.XAxis.Min)
        plot.XAxis.Max = plot.XAxis.Min + FLT_EPSILON;
    if (plot.YAxis.Max <= plot.YAxis.Min)
        plot.YAxis.Max = plot.YAxis.Min + FLT_EPSILON;

    // adaptive divisions
    if (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        plot.XAxis.Divisions = (int)std::round(0.003 * GPlt.bb_canvas.GetWidth());
        if (plot.XAxis.Divisions < 2)
            plot.XAxis.Divisions = 2;
    }
    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        plot.YAxis.Divisions = (int)std::round(0.003 * GPlt.bb_canvas.GetHeight());
        if (plot.YAxis.Divisions < 2)
            plot.YAxis.Divisions = 2;
    }

    GPlt.rend_x = (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_Grid) || Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickMarks) || Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  plot.XAxis.Divisions > 1;
    GPlt.rend_y = (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_Grid) || Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickMarks) || Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  plot.XAxis.Divisions > 1;

    // get ticks
    if (GPlt.rend_x)
        GetTicks(plot.XAxis.Min, plot.XAxis.Max, plot.XAxis.Divisions, plot.XAxis.Subdivisions, GPlt.x_ticks);
    if (GPlt.rend_y)
        GetTicks(plot.YAxis.Min, plot.YAxis.Max, plot.YAxis.Divisions, plot.YAxis.Subdivisions, GPlt.y_ticks);

    // label ticks
    if (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels))
        LabelTicks(GPlt.x_ticks);
    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels))
        LabelTicks(GPlt.y_ticks);

    // get max y-tick width
    float maxLabelWidth = 0;
    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        for (auto &yt : GPlt.y_ticks)
            maxLabelWidth = yt.size.x > maxLabelWidth ? yt.size.x : maxLabelWidth;
    }

    // grid bb
    const ImVec2 title_size = CalcTextSize(title, NULL, true);
    const float txt_off    = 5;
    const float txt_height = GetTextLineHeight();
    const float pad_top    = title_size.x > 0.0f ? txt_height + txt_off : 0;
    const float pad_bot    = (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels) ? txt_height + txt_off : 0) + (x_label ? txt_height + txt_off : 0);
    const float pad_left   = (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels) ? maxLabelWidth + txt_off : 0) + (y_label ? txt_height + txt_off : 0);
    GPlt.bb_grid          = ImRect(GPlt.bb_canvas.Min + ImVec2(pad_left, pad_top), GPlt.bb_canvas.Max - ImVec2(0, pad_bot));
    GPlt.hov_grid         = GPlt.bb_grid.Contains(IO.MousePos);

    // axis region bbs
    const ImRect xAxisRegion_bb(GPlt.bb_grid.Min + ImVec2(10, 0), {GPlt.bb_grid.Max.x, GPlt.bb_frame.Max.y});
    const bool   xAxisRegion_hovered = xAxisRegion_bb.Contains(IO.MousePos);
    const ImRect yAxisRegion_bb({GPlt.bb_frame.Min.x, GPlt.bb_grid.Min.y}, GPlt.bb_grid.Max - ImVec2(0, 10));
    const bool   yAxisRegion_hovered = yAxisRegion_bb.Contains(IO.MousePos);

    // TODO

    // get pixels for transforms
    GPlt.xform = ImRect(Flagged(plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.bb_grid.Max.x : GPlt.bb_grid.Min.x,
                        Flagged(plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.bb_grid.Min.y : GPlt.bb_grid.Max.y,
                        Flagged(plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.bb_grid.Min.x : GPlt.bb_grid.Max.x,
                        Flagged(plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.bb_grid.Max.y : GPlt.bb_grid.Min.y);

    GPlt.mx        = (GPlt.xform.Max.x - GPlt.xform.Min.x) / (plot.XAxis.Max - plot.XAxis.Min);
    GPlt.my        = (GPlt.xform.Max.y - GPlt.xform.Min.y) / (plot.YAxis.Max - plot.YAxis.Min);
    GPlt.cull_rect = ImRect(ImMin(GPlt.xform.Min.x, GPlt.xform.Max.x), ImMin(GPlt.xform.Min.y, GPlt.xform.Max.y), 
                             ImMax(GPlt.xform.Min.x, GPlt.xform.Max.x), ImMax(GPlt.xform.Min.y, GPlt.xform.Max.y));
    
    // set mouse position
    GPlt.mouse_pos.x = Remap(IO.MousePos.x, GPlt.xform.Min.x, GPlt.xform.Max.x, plot.XAxis.Min, plot.XAxis.Max);
    GPlt.mouse_pos.y = Remap(IO.MousePos.y, GPlt.xform.Min.y, GPlt.xform.Max.y, plot.YAxis.Min, plot.YAxis.Max);


    // TODO

    // focus window
    if ((IO.MouseClicked[0] || IO.MouseClicked[1]) && GPlt.hov_frame)
        FocusWindow(GetCurrentWindow());
            
    // RENDER

    // grid bg
    DrawList.AddRectFilled(GPlt.bb_grid.Min, GPlt.bb_grid.Max, GPlt.col_bg);

    // render axes
    ImGui::PushClipRect(GPlt.bb_grid.Min, GPlt.bb_grid.Max, true);

    // transform ticks
    if (GPlt.rend_x)
        TransformTicks(GPlt.x_ticks, plot.XAxis.Min, plot.XAxis.Max, GPlt.xform.Min.x, GPlt.xform.Max.x);
    if (GPlt.rend_y)
        TransformTicks(GPlt.y_ticks, plot.YAxis.Min, plot.YAxis.Max, GPlt.xform.Min.y, GPlt.xform.Max.y);

    // render grid
    if (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_Grid)) {
        for (auto &xt : GPlt.x_ticks)
            DrawList.AddLine({xt.pixels, GPlt.bb_grid.Min.y}, {xt.pixels, GPlt.bb_grid.Max.y}, xt.major ? GPlt.col_x1 : GPlt.col_x2, 1);
    }

    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_Grid)) {
        for (auto &yt : GPlt.y_ticks)
            DrawList.AddLine({GPlt.bb_grid.Min.x, yt.pixels}, {GPlt.bb_grid.Max.x, yt.pixels}, yt.major ? GPlt.col_y1 : GPlt.col_y2, 1);
    }

    // render ticks
    if (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &xt : GPlt.x_ticks)
            DrawList.AddLine({xt.pixels, GPlt.bb_grid.Max.y},{xt.pixels, GPlt.bb_grid.Max.y - (xt.major ? 10.0f : 5.0f)}, GPlt.col_border, 1);
    }
    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &yt : GPlt.y_ticks)
            DrawList.AddLine({GPlt.bb_grid.Min.x, yt.pixels}, {GPlt.bb_grid.Min.x + (yt.major ? 10.0f : 5.0f), yt.pixels}, GPlt.col_border, 1);
    }

    ImGui::PopClipRect();

    // render title
    if (title_size.x > 0.0f) {
        RenderText(ImVec2(GPlt.bb_canvas.GetCenter().x - title_size.x * 0.5f, GPlt.bb_canvas.Min.y), title, NULL, true);
    }

    // render labels
    if (Flagged(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.bb_frame.Min, GPlt.bb_frame.Max, true);
        for (auto &xt : GPlt.x_ticks)
            DrawList.AddText({xt.pixels - xt.size.x * 0.5f, GPlt.bb_grid.Max.y + txt_off}, GPlt.col_x_txt,
                             xt.txt.c_str());
        ImGui::PopClipRect();
    }
    if (x_label) {
        const ImVec2 xLabel_size = CalcTextSize(x_label);
        const ImVec2 xLabel_pos(GPlt.bb_grid.GetCenter().x - xLabel_size.x * 0.5f,
                                GPlt.bb_canvas.Max.y - txt_height);
        DrawList.AddText(xLabel_pos, GPlt.col_x_txt, x_label);
    }
    if (Flagged(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.bb_frame.Min, GPlt.bb_frame.Max, true);
        for (auto &yt : GPlt.y_ticks)
            DrawList.AddText({GPlt.bb_grid.Min.x - txt_off - yt.size.x, yt.pixels - 0.5f * yt.size.y},
                             GPlt.col_y_txt, yt.txt.c_str());
        ImGui::PopClipRect();
    }
    if (y_label) {
        const ImVec2 yLabel_size = CalcTextSizeVertical(y_label);
        const ImVec2 yLabel_pos(GPlt.bb_canvas.Min.x, GPlt.bb_grid.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, y_label, yLabel_pos, GPlt.col_y_txt);
    }


    GPlt.active = true;
    return true;
}

void EndPlot() {
    IM_ASSERT_USER_ERROR(GPlt.active == true, "Mismatched BeginPlot()/EndPlot()!");
    ImPlot &plot = *GPlt.plot;

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = GetIO();
    ImDrawList &      DrawList = *Window->DrawList;

    ImGui::PushClipRect(GPlt.bb_grid.Min, GPlt.bb_grid.Max, true);


    // render selection


    // render mouse pos
    if (Flagged(plot.Flags, ImPlotFlags_CursorLabel) && GPlt.hov_grid) {
        static char buffer[32];
        sprintf(buffer, "%.2f,%.2f", GPlt.mouse_pos.x, GPlt.mouse_pos.y);
        ImVec2 size = CalcTextSize(buffer);
        ImVec2 pos  = GPlt.bb_grid.Max - size - ImVec2(5, 5);
        DrawList.AddText(pos, GPlt.col_txt, buffer);
    }

    ImGui::PopClipRect();

    // render border
    DrawList.AddRect(GPlt.bb_grid.Min, GPlt.bb_grid.Max, GPlt.col_border);


    GPlt.active = false;
}



void PlotLine(const char* label, const float* xs, const float* ys, int size, 
              ImPlotSpec spec, const ImVec4& color_line, const ImVec4& color_fill, 
              bool* show, int offset, int stride)
{
    IM_ASSERT_USER_ERROR(GPlt.active == true, "PlotLine must be called between BeginPlot()/EndPlot()!");

    // TODO: Push label for legend

    if (show != nullptr && *show == false)
        return;

    ImDrawList & DrawList = *ImGui::GetWindowDrawList();

    const bool rend_line = color_line.w != 0;
    const bool rend_fill = color_fill.w != 0;

    ImU32 col_line = color_line.w == -1 ? NextColor() : GetColorU32(color_line);
    ImU32 col_fill = color_fill.w == -1 ? col_line    : GetColorU32(color_fill);

    ImGui::PushClipRect(GPlt.bb_grid.Min, GPlt.bb_grid.Max, true);

    // render line segments
    if (size > 1 && rend_line && Flagged(spec, ImPlotSpec_Solid)) {
        const float  thickness = 1.0f;
        const int    segments  = size - 1;
        const int    idx_count = segments * 6;
        const int    vtx_count = segments * 4;
        const ImVec2 uv        = DrawList._Data->TexUvWhitePixel;
        DrawList.PrimReserve(idx_count, vtx_count);
        int    i1 = offset;
        ImVec2 p1, p2;
        int segments_culled = 0;
        for (int s = 0; s < segments; ++s) {
            const int i2 = i1 + 1 == size ? 0 : i1 + 1;
            p1.x         = GPlt.xform.Min.x + GPlt.mx * (xs[i1] - GPlt.plot->XAxis.Min);
            p1.y         = GPlt.xform.Min.y + GPlt.my * (ys[i1] - GPlt.plot->YAxis.Min);
            p2.x         = GPlt.xform.Min.x + GPlt.mx * (xs[i2] - GPlt.plot->XAxis.Min);
            p2.y         = GPlt.xform.Min.y + GPlt.my * (ys[i2] - GPlt.plot->YAxis.Min);
            i1           = i2;
            if (GPlt.cull_rect.Contains(p1) || GPlt.cull_rect.Contains(p2)) {
                float dx = p2.x - p1.x;
                float dy = p2.y - p1.y;
                IM_NORMALIZE2F_OVER_ZERO(dx, dy);
                dx *= (thickness * 0.5f);
                dy *= (thickness * 0.5f);
                
                DrawList._VtxWritePtr[0].pos.x = p1.x + dy;
                DrawList._VtxWritePtr[0].pos.y = p1.y - dx;
                DrawList._VtxWritePtr[0].uv    = uv;
                DrawList._VtxWritePtr[0].col   = col_line;
                DrawList._VtxWritePtr[1].pos.x = p2.x + dy;
                DrawList._VtxWritePtr[1].pos.y = p2.y - dx;
                DrawList._VtxWritePtr[1].uv    = uv;
                DrawList._VtxWritePtr[1].col   = col_line;
                DrawList._VtxWritePtr[2].pos.x = p2.x - dy;
                DrawList._VtxWritePtr[2].pos.y = p2.y + dx;
                DrawList._VtxWritePtr[2].uv    = uv;
                DrawList._VtxWritePtr[2].col   = col_line;
                DrawList._VtxWritePtr[3].pos.x = p1.x - dy;
                DrawList._VtxWritePtr[3].pos.y = p1.y + dx;
                DrawList._VtxWritePtr[3].uv    = uv;
                DrawList._VtxWritePtr[3].col   = col_line;
                DrawList._VtxWritePtr += 4;

                DrawList._IdxWritePtr[0] = (ImDrawIdx)(DrawList._VtxCurrentIdx);
                DrawList._IdxWritePtr[1] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 1);
                DrawList._IdxWritePtr[2] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 2);
                DrawList._IdxWritePtr[3] = (ImDrawIdx)(DrawList._VtxCurrentIdx);
                DrawList._IdxWritePtr[4] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 2);
                DrawList._IdxWritePtr[5] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 3);
                DrawList._IdxWritePtr += 6;
                DrawList._VtxCurrentIdx += 4;
            }
            else {
                segments_culled++;
            }
        }
        if (segments_culled > 0) 
            DrawList.PrimUnreserve(segments_culled * 6, segments_culled * 4); 
    }

    // render markers
    // TODO: Coarse check for any marker flags
    for (int i = 0; i < size; ++i) {
        ImVec2 c;
        c.x = GPlt.xform.Min.x + GPlt.mx * (xs[i] - GPlt.plot->XAxis.Min);
        c.y = GPlt.xform.Min.y + GPlt.my * (ys[i] - GPlt.plot->YAxis.Min);
        if (GPlt.cull_rect.Contains(c)) {
            if (Flagged(spec, ImPlotSpec_Circle)) {
                if (rend_fill)
                    DrawList.AddCircleFilled(c, 4, col_fill, 10);
                if (rend_line)
                    DrawList.AddCircle(c, 4, col_line, 10);
            }
            if (Flagged(spec, ImPlotSpec_Diamond)) {
                if (rend_fill)
                    DrawList.AddCircleFilled(c, 4, col_fill, 4);
                if (rend_line)
                    DrawList.AddCircle(c, 4, col_line, 4);     
           }
        }
    }    
    ImGui::PopClipRect();
}



}  // namespace ImGui
