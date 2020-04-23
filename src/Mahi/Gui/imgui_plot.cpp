#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <Mahi/Gui/imgui_plot.hpp>
#include <imgui_internal.h>
#include <iostream>

#define IM_NORMALIZE2F_OVER_ZERO(VX, VY)                                                           \
    {                                                                                              \
        float d2 = VX * VX + VY * VY;                                                              \
        if (d2 > 0.0f) {                                                                           \
            float inv_len = 1.0f / ImSqrt(d2);                                                     \
            VX *= inv_len;                                                                         \
            VY *= inv_len;                                                                         \
        }                                                                                          \
    }

ImPlotStyle::ImPlotStyle() {
    Spec = ImPlotSpec_SolidLine;
    MarkerSize = 5;
    LineWeight = 1;
    Colors[ImPlotCol_Line]       = IM_COL_AUTO;
    Colors[ImPlotCol_Fill]       = IM_COL_AUTO;
    Colors[ImPlotCol_FrameBg]    = IM_COL_AUTO;
    Colors[ImPlotCol_PlotBg]     = IM_COL_AUTO;
    Colors[ImPlotCol_PlotBorder] = IM_COL_AUTO;
    Colors[ImPlotCol_XAxis]      = IM_COL_AUTO;
    Colors[ImPlotCol_YAxis]      = IM_COL_AUTO;
    Colors[ImPlotCol_Selection]  = ImVec4(1,1,0,1);
}

namespace ImGui {

namespace {

//=============================================================================
// General Utils
//=============================================================================

/// Returns true if a flag is set
template <typename TSet, typename TFlag>
inline bool HasFlag(TSet set, TFlag flag) {
    return (set & flag) == flag;
}

/// Flips a flag in a flagset
template <typename TSet, typename TFlag> 
inline void FlipFlag(TSet& set, TFlag flag) {
    HasFlag(set, flag) ? set &= ~flag : set |= flag;
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
    int expv = (int)floor(log10(x));
    f = x / ImPow(10.0, (double)expv); /* between 1 and 10 */
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
    return nf * ImPow(10.0, expv);
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
/// Returns the next unused default plot color
ImVec4 NextColor() {
    static ImVec4 default_colors[10] = {
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
    auto col  = default_colors[next_color];
    next_color = (next_color + 1) % 10;
    return col;
}

//=============================================================================
// Structs
//=============================================================================

/// Tick mark info
struct ImTick {
    ImTick(double value, bool major, bool render_label = true) { 
        PlotPos = value;
        Major = major;
        RenderLabel = render_label;
    }
    double PlotPos;
    float  PixelPos;
    bool   Major;
    ImVec2 Size;
    int    TextOffset;
    bool   RenderLabel;
};

struct ImPlotItem {
    ImPlotItem() { Show = true; Color = NextColor(); NameOffset = -1; Active = true; ID = 0;  }
    ~ImPlotItem() { ID = 0; }
    bool Show;
    ImVec4 Color;
    int NameOffset;
    bool Active;
    ImGuiID ID;
};

/// Plot axis structure. You shouldn't need to construct this!
struct ImPlotAxis {
    ImPlotAxis() { 
        Dragging = false;
        ZoomRate = 0.1f;
        Min = 0; 
        Max = 1; 
        Divisions = 3; 
        Subdivisions = 10; 
        Flags = ImPlotAxisFlags_Default; 
    }
    bool Dragging;
    float ZoomRate;
    float Min;
    float Max;
    int Divisions;
    int Subdivisions;
    ImPlotAxisFlags Flags;
};

/// Holds Plot state information that must persist between frames
struct ImPlot {
    ImPlot() {
        Selecting = false;
        SelectStart = {0,0};
        Flags = ImPlotFlags_Default;
    }
    ImPool<ImPlotItem> Items;

    ImRect BB_Legend;
    bool Selecting;
    ImVec2 SelectStart;
    ImPlotAxis XAxis;
    ImPlotAxis YAxis;
    ImPlotFlags Flags;
};

/// Holds Plot state information that must persist only between calls to BeginPlot()/EndPlot()
struct ImPlotContext {
    ImPlotContext() {
        CurrentPlot = NULL;
    }
    /// ALl Plots    
    ImPool<ImPlot> PlotData;
    /// Current Plot
    ImPlot* CurrentPlot;

    // Legend

    ImVector<int> _LegendIndices;    
    ImGuiTextBuffer _LegendLabels;

    const char* GetLegendLabel(int i) const {
        ImPlotItem* item  = CurrentPlot->Items.GetByIndex(_LegendIndices[i]);
        IM_ASSERT(item->NameOffset != -1 && item->NameOffset < _LegendLabels.Buf.Size);
        return _LegendLabels.Buf.Data + item->NameOffset;
    }

    ImPlotItem* GetLegendItem(int i) {
        return CurrentPlot->Items.GetByIndex(_LegendIndices[i]);
    }

    int GetLegendCount() const {
        return _LegendIndices.size();
    }

    ImPlotItem* RegisterItem(const char* label_id) {
        ImGuiID id = ImGui::GetID(label_id);
        ImPlotItem* item = CurrentPlot->Items.GetOrAddByKey(id);
        int idx = CurrentPlot->Items.GetIndex(item);
        item->Active = true;
        item->ID = id;
        _LegendIndices.push_back(idx);
        item->NameOffset = _LegendLabels.size();
        _LegendLabels.append(label_id, label_id + strlen(label_id) + 1);
        if (item->Show)
            VisibleItemCount++;
        return item;
    }

    // Bounding regions    
    ImRect BB_Frame;
    ImRect BB_Canvas;
    ImRect BB_Grid;

    // Hover states
    bool Hov_Frame;
    bool Hov_Grid;

    // Colors
    ImU32 Col_Frame, Col_Bg, Col_Border, 
          Col_Txt, Col_TxtDis, 
          Col_SlctBg, Col_SlctBd,
          Col_XMajor, Col_XMinor, Col_XTxt,
          Col_YMajor, Col_YMinor, Col_YTxt;    

    // Tick marks buffers  
    ImVector<ImTick> XTicks,  YTicks;
    ImGuiTextBuffer XTickLabels, YTickLabels;

    // Transformations
    ImRect PixelRange;
    float Mx, My;
    float LogDenX, LogDenY;
    inline ImVec2 ToPixels(float x, float y) {
        ImVec2 out;
        if (HasFlag(CurrentPlot->XAxis.Flags, ImPlotAxisFlags_LogScale)) {
            float t = log10(x / CurrentPlot->XAxis.Min) / LogDenX;   
            x       = ImLerp(CurrentPlot->XAxis.Min, CurrentPlot->XAxis.Max, t);
        }             
        if (HasFlag(CurrentPlot->YAxis.Flags, ImPlotAxisFlags_LogScale)) {
            float t = log10(y / CurrentPlot->YAxis.Min) / LogDenY;   
            y       = ImLerp(CurrentPlot->YAxis.Min, CurrentPlot->YAxis.Max, t);
        }
        out.x = PixelRange.Min.x + Mx * (x - CurrentPlot->XAxis.Min);
        out.y = PixelRange.Min.y + My * (y - CurrentPlot->YAxis.Min);
        return out;
    }
    inline ImVec2 ToPixels(const ImVec2& in) {
        return ToPixels(in.x, in.y);
    }
    inline ImVec2 FromPixels(const ImVec2& in) {
        ImVec2 out;
        out.x = (in.x - PixelRange.Min.x) / Mx + CurrentPlot->XAxis.Min;
        out.y = (in.y - PixelRange.Min.y) / My + CurrentPlot->YAxis.Min;
        if (HasFlag(CurrentPlot->XAxis.Flags, ImPlotAxisFlags_LogScale)) {
            float t = (out.x - CurrentPlot->XAxis.Min) / (CurrentPlot->XAxis.Max - CurrentPlot->XAxis.Min);
            out.x = pow(10, t * LogDenX) * CurrentPlot->XAxis.Min;
        }
        if (HasFlag(CurrentPlot->YAxis.Flags, ImPlotAxisFlags_LogScale)) {
            float t = (out.y - CurrentPlot->YAxis.Min) / (CurrentPlot->YAxis.Max - CurrentPlot->YAxis.Min);
            out.y = pow(10, t * LogDenY) * CurrentPlot->YAxis.Min;
        }
        return out;
    }
    inline void UpdateTransforms() {
        // get pixels for transforms
        PixelRange = ImRect(HasFlag(CurrentPlot->XAxis.Flags, ImPlotAxisFlags_Invert) ? BB_Grid.Max.x : BB_Grid.Min.x,
                                 HasFlag(CurrentPlot->YAxis.Flags, ImPlotAxisFlags_Invert) ? BB_Grid.Min.y : BB_Grid.Max.y,
                                 HasFlag(CurrentPlot->XAxis.Flags, ImPlotAxisFlags_Invert) ? BB_Grid.Min.x : BB_Grid.Max.x,
                                 HasFlag(CurrentPlot->YAxis.Flags, ImPlotAxisFlags_Invert) ? BB_Grid.Max.y : BB_Grid.Min.y);   

        Mx       = (PixelRange.Max.x - PixelRange.Min.x) / (CurrentPlot->XAxis.Max - CurrentPlot->XAxis.Min);
        My       = (PixelRange.Max.y - PixelRange.Min.y) / (CurrentPlot->YAxis.Max - CurrentPlot->YAxis.Min);
        LogDenX  = log10(CurrentPlot->XAxis.Max / CurrentPlot->XAxis.Min);
        LogDenY  = log10(CurrentPlot->YAxis.Max / CurrentPlot->YAxis.Min);
    }

    // Data extents
    ImRect Extents;
    bool FitThisFrame;
    int VisibleItemCount;
    // Render flags
    bool RenderX, RenderY;
    // Mouse pos
    ImVec2 LastMousePos;
    // Style
    ImPlotStyle Style;
    
};

/// Global plot context
static ImPlotContext GPlt;

//=============================================================================
// Tick Utils
//=============================================================================

inline void GetTicks(float tMin, float tMax, int nMajor, int nMinor, bool logscale, ImVector<ImTick> &out) {
    out.shrink(0);
    if (logscale) {
        if (tMin <= 0 || tMax <= 0)
            return;
        int exp_min = (int)(ImFloor(log10(tMin)));
        int exp_max = (int)(ImCeil(log10(tMax)));
        for (int e = exp_min; e < exp_max + 1; ++e) {
            double major1 = ImPow(10, (double)(e));
            double major2 = ImPow(10, (double)(e + 1));
            double interval = (major2 - major1) / 9;
            if (major1 >= tMin && major1 <= tMax)
                out.push_back(ImTick(major1, true));
            for (int i = 1; i < 9; ++i) {
                double minor = major1 + i * interval;
                if (minor >= tMin && minor <= tMax)
                    out.push_back(ImTick(minor, false, false));
            }
        }
    }
    else {
        const double range    = NiceNum(tMax - tMin, 0);
        const double interval = NiceNum(range / (nMajor - 1), 1);
        const double graphmin = floor(tMin / interval) * interval;
        const double graphmax = ceil(tMax / interval) * interval;
        for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval) {
            if (major >= tMin && major <= tMax)
                out.push_back(ImTick(major, true));
            for (int i = 1; i < nMinor; ++i) {
                double minor = major + i * interval / nMinor;
                if (minor >= tMin && minor <= tMax)
                    out.push_back(ImTick(minor, false));
            }
        }
    }
}

inline void LabelTicks(ImVector<ImTick> &ticks, ImGuiTextBuffer& buffer) {
    buffer.Buf.resize(0);
    char temp[32];
    for (auto &tk : ticks) {
        if (tk.RenderLabel) {
            tk.TextOffset = buffer.size();
            sprintf(temp, "%g", tk.PlotPos);
            buffer.append(temp, temp + strlen(temp) + 1);
            tk.Size = CalcTextSize(buffer.Buf.Data + tk.TextOffset);
        }
    }
}

} // private namespace

//=============================================================================
// BeginPlot()
//=============================================================================

bool BeginPlot(const char* title, const char* x_label, const char* y_label, const ImVec2& size, ImPlotFlags flags, ImPlotAxisFlags x_flags, ImPlotAxisFlags y_flags) {

    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot == NULL, "Mismatched BeginPlot()/EndPlot()!");

    // Front Matter -----------------------------------------------------------

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    const ImGuiStyle &Style    = G.Style;
    const ImGuiIO &   IO       = GetIO();
    ImDrawList &      DrawList = *Window->DrawList;
    const ImGuiID     ID       = Window->GetID(title);
    
    bool just_created = GPlt.PlotData.GetByKey(ID) == NULL;    
    GPlt.CurrentPlot = GPlt.PlotData.GetOrAddByKey(ID);
    ImPlot &plot = *GPlt.CurrentPlot;

    if (just_created) {
        plot.Flags       = flags;
        plot.XAxis.Flags = x_flags;
        plot.YAxis.Flags = y_flags;
    }

    // Colors -----------------------------------------------------------------

    GPlt.Col_Frame  = GPlt.Style.Colors[ImPlotCol_FrameBg].w     == -1 ? GetColorU32(ImGuiCol_FrameBg)    : GetColorU32(GPlt.Style.Colors[ImPlotCol_FrameBg]);
    GPlt.Col_Bg     = GPlt.Style.Colors[ImPlotCol_PlotBg].w      == -1 ? GetColorU32(ImGuiCol_WindowBg)   : GetColorU32(GPlt.Style.Colors[ImPlotCol_PlotBg]);
    GPlt.Col_Border = GPlt.Style.Colors[ImPlotCol_PlotBorder].w  == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(GPlt.Style.Colors[ImPlotCol_PlotBorder]);

    const ImVec4 col_xAxis = GPlt.Style.Colors[ImPlotCol_XAxis].w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : GPlt.Style.Colors[ImPlotCol_XAxis];
    GPlt.Col_XMajor = GetColorU32(col_xAxis);
    GPlt.Col_XMinor = GetColorU32(col_xAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.Col_XTxt   = GetColorU32({col_xAxis.x, col_xAxis.y, col_xAxis.z, 1});

    const ImVec4 col_yAxis = GPlt.Style.Colors[ImPlotCol_YAxis].w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : GPlt.Style.Colors[ImPlotCol_YAxis];
    GPlt.Col_YMajor = GetColorU32(col_yAxis);
    GPlt.Col_YMinor = GetColorU32(col_yAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.Col_YTxt   = GetColorU32({col_yAxis.x, col_yAxis.y, col_yAxis.z, 1});

    GPlt.Col_Txt    = GetColorU32(ImGuiCol_Text);
    GPlt.Col_TxtDis = GetColorU32(ImGuiCol_TextDisabled);
    GPlt.Col_SlctBg = GetColorU32(GPlt.Style.Colors[ImPlotCol_Selection] * ImVec4(1,1,1,0.25f));
    GPlt.Col_SlctBd = GetColorU32(GPlt.Style.Colors[ImPlotCol_Selection]);

    // BB AND HOVER -----------------------------------------------------------

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    GPlt.BB_Frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(GPlt.BB_Frame);
    if (!ItemAdd(GPlt.BB_Frame, 0, &GPlt.BB_Frame)) {
        GPlt.CurrentPlot = NULL;
        return false;
    }
    GPlt.Hov_Frame = ItemHoverable(GPlt.BB_Frame, ID);
    RenderFrame(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, GPlt.Col_Frame, true, Style.FrameRounding);

    // canvas bb
    GPlt.BB_Canvas = ImRect(GPlt.BB_Frame.Min + Style.WindowPadding, GPlt.BB_Frame.Max - Style.WindowPadding);

    // constrain axes
    if (plot.XAxis.Max <= plot.XAxis.Min)
        plot.XAxis.Max = plot.XAxis.Min + FLT_EPSILON;
    if (plot.YAxis.Max <= plot.YAxis.Min)
        plot.YAxis.Max = plot.YAxis.Min + FLT_EPSILON;

    // adaptive divisions
    if (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        plot.XAxis.Divisions = (int)std::round(0.003 * GPlt.BB_Canvas.GetWidth());
        if (plot.XAxis.Divisions < 2)
            plot.XAxis.Divisions = 2;
    }
    if (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        plot.YAxis.Divisions = (int)std::round(0.003 * GPlt.BB_Canvas.GetHeight());
        if (plot.YAxis.Divisions < 2)
            plot.YAxis.Divisions = 2;
    }

    GPlt.RenderX = (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_GridLines) || 
                    HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickMarks) || 
                    HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  plot.XAxis.Divisions > 1;
    GPlt.RenderY = (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_GridLines) || 
                    HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickMarks) || 
                    HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  plot.YAxis.Divisions > 1;

    // get ticks
    if (GPlt.RenderX)
        GetTicks(plot.XAxis.Min, plot.XAxis.Max, plot.XAxis.Divisions, plot.XAxis.Subdivisions, HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LogScale), GPlt.XTicks);
    if (GPlt.RenderY)
        GetTicks(plot.YAxis.Min, plot.YAxis.Max, plot.YAxis.Divisions, plot.YAxis.Subdivisions, HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LogScale), GPlt.YTicks);

    // label ticks
    if (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels))
        LabelTicks(GPlt.XTicks, GPlt.XTickLabels);

    float max_label_width = 0;
    if (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        LabelTicks(GPlt.YTicks, GPlt.YTickLabels);
        for (auto &yt : GPlt.YTicks)
            max_label_width = yt.Size.x > max_label_width ? yt.Size.x : max_label_width;
    }

    // grid bb
    const ImVec2 title_size = CalcTextSize(title, NULL, true);
    const float txt_off     = 5;
    const float txt_height  = GetTextLineHeight();
    const float pad_top     = title_size.x > 0.0f ? txt_height + txt_off : 0;
    const float pad_bot     = (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels) ? txt_height + txt_off : 0) + (x_label ? txt_height + txt_off : 0);
    const float pad_left    = (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels) ? max_label_width + txt_off : 0) + (y_label ? txt_height + txt_off : 0);
    GPlt.BB_Grid            = ImRect(GPlt.BB_Canvas.Min + ImVec2(pad_left, pad_top), GPlt.BB_Canvas.Max - ImVec2(0, pad_bot));
    GPlt.Hov_Grid           = GPlt.BB_Grid.Contains(IO.MousePos);

    // axis region bbs
    const ImRect xAxisRegion_bb(GPlt.BB_Grid.Min + ImVec2(10, 0), {GPlt.BB_Grid.Max.x, GPlt.BB_Frame.Max.y});
    const bool   xAxisRegion_hovered = xAxisRegion_bb.Contains(IO.MousePos);
    const ImRect yAxisRegion_bb({GPlt.BB_Frame.Min.x, GPlt.BB_Grid.Min.y}, GPlt.BB_Grid.Max - ImVec2(0, 10));
    const bool   yAxisRegion_hovered = yAxisRegion_bb.Contains(IO.MousePos);

    // legend hovered from last frame
    const bool legend_hovered = HasFlag(plot.Flags, ImPlotFlags_Legend) ? GPlt.Hov_Frame && plot.BB_Legend.Contains(IO.MousePos) : false;

    // DRAG INPUT -------------------------------------------------------------


    // end drags
    if (plot.XAxis.Dragging && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        plot.XAxis.Dragging             = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0; 
    }
    if (plot.YAxis.Dragging && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        plot.YAxis.Dragging             = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0;
    }
    // do drag
    if (plot.XAxis.Dragging || plot.YAxis.Dragging) {
        GPlt.UpdateTransforms();
        bool xLocked = HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin) && HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax);
        if (!xLocked && plot.XAxis.Dragging) {
            float delX = IO.MouseDelta.x / GPlt.Mx;
            if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.XAxis.Min -= delX;
            if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.XAxis.Max -= delX;
        }
        bool yLocked = HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin) && HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax);
        if (!yLocked && plot.YAxis.Dragging) {
            float delY = -IO.MouseDelta.y / GPlt.My;
            if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.YAxis.Min += delY;
            if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.YAxis.Max += delY;
        }

        if ((xLocked && yLocked) || (xLocked && plot.XAxis.Dragging && !plot.YAxis.Dragging) || (yLocked && plot.YAxis.Dragging && !plot.XAxis.Dragging))
            ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
        else if (xLocked || (!plot.XAxis.Dragging && plot.YAxis.Dragging))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        else if (yLocked || (!plot.YAxis.Dragging && plot.XAxis.Dragging))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    // start drag
    if (GPlt.Hov_Frame && xAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !plot.Selecting && !legend_hovered)
        plot.XAxis.Dragging = true;
    if (GPlt.Hov_Frame && yAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !plot.Selecting && !legend_hovered)
        plot.YAxis.Dragging = true;

    // SCROLL INPUT -----------------------------------------------------------

    if (GPlt.Hov_Frame && (xAxisRegion_hovered || yAxisRegion_hovered) && IO.MouseWheel != 0) {
        GPlt.UpdateTransforms();
        float xRange    = plot.XAxis.Max - plot.XAxis.Min;
        float yRange    = plot.YAxis.Max - plot.YAxis.Min;
        float xZoomRate = plot.XAxis.ZoomRate;
        float yZoomRate = plot.YAxis.ZoomRate;
        if (IO.MouseWheel > 0) {
            xZoomRate = (-xZoomRate) / (1.0f + (2.0f * xZoomRate));
            yZoomRate = (-yZoomRate) / (1.0f + (2.0f * yZoomRate));
        }
        // Determine center of zoom
        ImVec2 zoomCenter;
        if (xAxisRegion_hovered && yAxisRegion_hovered) {
            // Mouse pointer is in plot -> zoom in on mouse pointer
            zoomCenter = GPlt.FromPixels(IO.MousePos);
        } else {
            // Zoom in on center of plot
            zoomCenter.x = 0.5f * (plot.XAxis.Max + plot.XAxis.Min);
            zoomCenter.y = 0.5f * (plot.YAxis.Max + plot.YAxis.Min);
        }

        if (xAxisRegion_hovered && yAxisRegion_hovered) {
            // x axis
            if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin) && !HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax)) {
                plot.XAxis.Min = zoomCenter.x + (plot.XAxis.Min - zoomCenter.x) * (1.0f + (2.0f * xZoomRate));
                plot.XAxis.Max = zoomCenter.x + (plot.XAxis.Max - zoomCenter.x) * (1.0f + (2.0f * xZoomRate));
            } 
            else if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.XAxis.Min -= xZoomRate * xRange;
            else if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.XAxis.Max += xZoomRate * xRange;
            // y axis
            if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin) && !HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax)) {
                plot.YAxis.Min = zoomCenter.y + (plot.YAxis.Min - zoomCenter.y) * (1.0f + (2.0f * yZoomRate));
                plot.YAxis.Max = zoomCenter.y + (plot.YAxis.Max - zoomCenter.y) * (1.0f + (2.0f * yZoomRate));
            } 
            else if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.YAxis.Min -= yZoomRate * yRange;
            else if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.YAxis.Max += yZoomRate * yRange;
        } 
        else if (xAxisRegion_hovered) {
            if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.XAxis.Min -= xZoomRate * xRange;
            if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.XAxis.Max += xZoomRate * xRange;
        } 
        else if (yAxisRegion_hovered) {
            if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                plot.YAxis.Min -= yZoomRate * yRange;
            if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                plot.YAxis.Max += yZoomRate * yRange;
        }
    }

    // SELECTION --------------------------------------------------------------

    // confirm selection
    if (plot.Selecting && (IO.MouseReleased[1] || !IO.MouseDown[1])) {
        ImVec2 select_size = plot.SelectStart - IO.MousePos;
        if (std::abs(select_size.x) > 2 && std::abs(select_size.y) > 2) {
            ImVec2 p1 = GPlt.FromPixels(plot.SelectStart);
            ImVec2 p2 = GPlt.FromPixels(IO.MousePos);
            plot.XAxis.Min = ImMin(p1.x, p2.x);
            plot.XAxis.Max = ImMax(p1.x, p2.x);
            plot.YAxis.Min = ImMin(p1.y, p2.y);
            plot.YAxis.Max = ImMax(p1.y, p2.y);
        }
        plot.Selecting = false;
    }
    // cancel selection
    if (plot.Selecting && (IO.MouseClicked[0] || IO.MouseDown[0])) {
        plot.Selecting = false;
    }
    // begin selection
    if (GPlt.Hov_Frame && GPlt.Hov_Grid && IO.MouseClicked[1] && HasFlag(plot.Flags, ImPlotFlags_Selection)) {
        plot.SelectStart = IO.MousePos;
        plot.Selecting    = true;
    }
    
    // DBL CLICK --------------------------------------------------------------

    if (GPlt.Hov_Frame && GPlt.Hov_Grid && IO.MouseDoubleClicked[0]) 
        GPlt.FitThisFrame = true;
    else
        GPlt.FitThisFrame = false;     

    // FOCUS ------------------------------------------------------------------

    // focus window
    if ((IO.MouseClicked[0] || IO.MouseClicked[1]) && GPlt.Hov_Frame)
        FocusWindow(GetCurrentWindow());           

    GPlt.UpdateTransforms();

    // set mouse position
    GPlt.LastMousePos = GPlt.FromPixels(IO.MousePos);

    // RENDER -----------------------------------------------------------------

    // grid bg
    DrawList.AddRectFilled(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, GPlt.Col_Bg);

    // render axes
    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    // transform ticks
    if (GPlt.RenderX) {
        for (auto& xt : GPlt.XTicks)
            xt.PixelPos = GPlt.ToPixels((float)xt.PlotPos, 0).x;
    }
    if (GPlt.RenderY) {
        for (auto& yt : GPlt.YTicks)
            yt.PixelPos = GPlt.ToPixels(0, (float)yt.PlotPos).y;
    }

    // render grid
    if (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_GridLines)) {
        for (auto &xt : GPlt.XTicks)
            DrawList.AddLine({xt.PixelPos, GPlt.BB_Grid.Min.y}, {xt.PixelPos, GPlt.BB_Grid.Max.y}, xt.Major ? GPlt.Col_XMajor : GPlt.Col_XMinor, 1);
    }

    if (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_GridLines)) {
        for (auto &yt : GPlt.YTicks)
            DrawList.AddLine({GPlt.BB_Grid.Min.x, yt.PixelPos}, {GPlt.BB_Grid.Max.x, yt.PixelPos}, yt.Major ? GPlt.Col_YMajor : GPlt.Col_YMinor, 1);
    }

    ImGui::PopClipRect();

    // render title
    if (title_size.x > 0.0f) {
        RenderText(ImVec2(GPlt.BB_Canvas.GetCenter().x - title_size.x * 0.5f, GPlt.BB_Canvas.Min.y), title, NULL, true);
    }

    // render labels
    if (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, true);
        for (auto &xt : GPlt.XTicks) {
            if (xt.RenderLabel && xt.PixelPos >= GPlt.BB_Grid.Min.x && xt.PixelPos <= GPlt.BB_Grid.Max.x)
                DrawList.AddText({xt.PixelPos - xt.Size.x * 0.5f, GPlt.BB_Grid.Max.y + txt_off}, GPlt.Col_XTxt, GPlt.XTickLabels.Buf.Data + xt.TextOffset);
        }
        ImGui::PopClipRect();
    }
    if (x_label) {
        const ImVec2 xLabel_size = CalcTextSize(x_label);
        const ImVec2 xLabel_pos(GPlt.BB_Grid.GetCenter().x - xLabel_size.x * 0.5f,
                                GPlt.BB_Canvas.Max.y - txt_height);
        DrawList.AddText(xLabel_pos, GPlt.Col_XTxt, x_label);
    }
    if (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, true);
        for (auto &yt : GPlt.YTicks) {
            if (yt.RenderLabel && yt.PixelPos >= GPlt.BB_Grid.Min.y && yt.PixelPos <= GPlt.BB_Grid.Max.y)
                DrawList.AddText({GPlt.BB_Grid.Min.x - txt_off - yt.Size.x, yt.PixelPos - 0.5f * yt.Size.y}, GPlt.Col_YTxt, GPlt.YTickLabels.Buf.Data + yt.TextOffset);
        }
        ImGui::PopClipRect();
    }
    if (y_label) {
        const ImVec2 yLabel_size = CalcTextSizeVertical(y_label);
        const ImVec2 yLabel_pos(GPlt.BB_Canvas.Min.x, GPlt.BB_Grid.GetCenter().y + yLabel_size.y * 0.5f);
        AddTextVertical(&DrawList, y_label, yLabel_pos, GPlt.Col_YTxt);
    }

    // PREP -------------------------------------------------------------------

    // push plot ID into stack
    PushID(ID);
    // Deactivate all existing items
    for (int i = 0; i < plot.Items.GetSize(); ++i) 
        plot.Items.GetByIndex(i)->Active = false;  
    // reset items count
    GPlt.VisibleItemCount = 0;
    // reset extents
    GPlt.Extents.Min.x = INFINITY;
    GPlt.Extents.Min.y = INFINITY;
    GPlt.Extents.Max.x = -INFINITY;
    GPlt.Extents.Max.y = -INFINITY;
    // clear item names
    GPlt._LegendLabels.Buf.resize(0);
    return true;
}

//=============================================================================
// Context Menu
//=============================================================================

inline void AxisMenu(ImPlotAxis& Axis) {
     ImGui::PushItemWidth(75);
    bool lock_min = HasFlag(Axis.Flags, ImPlotAxisFlags_LockMin);
    bool lock_max = HasFlag(Axis.Flags, ImPlotAxisFlags_LockMax);
    bool invert   = HasFlag(Axis.Flags, ImPlotAxisFlags_Invert);
    bool logscale = HasFlag(Axis.Flags, ImPlotAxisFlags_LogScale);
    bool grid     = HasFlag(Axis.Flags, ImPlotAxisFlags_GridLines);
    bool ticks    = HasFlag(Axis.Flags, ImPlotAxisFlags_TickMarks);
    bool labels   = HasFlag(Axis.Flags, ImPlotAxisFlags_TickLabels);
    if (ImGui::Checkbox("##LockMin", &lock_min)) 
        FlipFlag(Axis.Flags, ImPlotAxisFlags_LockMin);
    ImGui::SameLine();
    if (lock_min) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
    }
    ImGui::DragFloat("Min", &Axis.Min, 0.01f + 0.01f * (Axis.Max - Axis.Min), -INFINITY, Axis.Max - FLT_EPSILON);
    if (lock_min) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();    }

    if (ImGui::Checkbox("##LockMax", &lock_max))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_LockMax);
    ImGui::SameLine();
    if (lock_max) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);    }
    ImGui::DragFloat("Max", &Axis.Max, 0.01f + 0.01f * (Axis.Max - Axis.Min), Axis.Min + FLT_EPSILON, INFINITY);
    if (lock_max) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
    ImGui::Separator();
    if (ImGui::Checkbox("Invert", &invert))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_Invert);
    if (ImGui::Checkbox("Log Scale", &logscale))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_LogScale);
    ImGui::Separator();
    if (ImGui::Checkbox("Grid Lines", &grid))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_GridLines);
    if (ImGui::Checkbox("Tick Marks", &ticks))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_TickMarks);
    if (ImGui::Checkbox("Labels", &labels))
        FlipFlag(Axis.Flags, ImPlotAxisFlags_TickLabels);
}

void PlotContextMenu(ImPlot& plot) {
    if (ImGui::BeginMenu("X-Axis")) {            
        AxisMenu(plot.XAxis);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Y-Axis")) {
        AxisMenu(plot.YAxis);
        ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Legend",NULL,HasFlag(plot.Flags, ImPlotFlags_Legend))) {
        FlipFlag(plot.Flags, ImPlotFlags_Legend);
    }
    if (ImGui::MenuItem("Crosshairs",NULL,HasFlag(plot.Flags, ImPlotFlags_Crosshairs))) {
        FlipFlag(plot.Flags, ImPlotFlags_Crosshairs);
    }
}

//=============================================================================
// EndPlot()
//=============================================================================

void EndPlot() {

    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot != NULL, "Mismatched BeginPlot()/EndPlot()!");

    ImPlot &plot = *GPlt.CurrentPlot;

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    ImDrawList & DrawList = *Window->DrawList;
    const ImGuiIO &   IO = GetIO();

    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    // render legend
    const float txt_ht = GetTextLineHeight();
    const ImVec2 legend_offset(10, 10);
    const ImVec2 legend_padding(5, 5);
    const float  legend_icon_size = txt_ht;
    ImRect legend_content_bb;
    int nItems = GPlt.GetLegendCount();
    bool legend_hovered = false;
    if (HasFlag(plot.Flags, ImPlotFlags_Legend) && nItems > 0) {
        // get max width
        float max_label_width = 0;
        for (int i = 0; i < nItems; ++i) {
            const char* label = GPlt.GetLegendLabel(i);
            auto labelWidth = CalcTextSize(label, NULL, true);
            max_label_width   = labelWidth.x > max_label_width ? labelWidth.x : max_label_width;
        }
        legend_content_bb = ImRect(GPlt.BB_Grid.Min + legend_offset, GPlt.BB_Grid.Min + legend_offset + ImVec2(max_label_width, nItems * txt_ht));
        plot.BB_Legend    = ImRect(legend_content_bb.Min, legend_content_bb.Max + legend_padding * 2 + ImVec2(legend_icon_size, 0));
        legend_hovered = HasFlag(plot.Flags, ImPlotFlags_Legend) ? GPlt.Hov_Frame && plot.BB_Legend.Contains(IO.MousePos) : false;
        // render legend box
        DrawList.AddRectFilled(plot.BB_Legend.Min, plot.BB_Legend.Max, GetColorU32(ImGuiCol_PopupBg));
        DrawList.AddRect(plot.BB_Legend.Min, plot.BB_Legend.Max, GPlt.Col_Border);
        // render each legend item
        for (int i = 0; i < nItems; ++i) {
            ImPlotItem* item = GPlt.GetLegendItem(i);
            ImRect icon_bb;
            icon_bb.Min = legend_content_bb.Min + legend_padding + ImVec2(0, i * txt_ht) + ImVec2(2, 2);
            icon_bb.Max = legend_content_bb.Min + legend_padding + ImVec2(0, i * txt_ht) + ImVec2(legend_icon_size - 2, legend_icon_size - 2);
            ImU32 iconColor;
            if (legend_hovered && icon_bb.Contains(IO.MousePos)) {
                auto colAlpha = item->Color;
                colAlpha.w    = 0.5f;
                iconColor     = item->Show ? GetColorU32(colAlpha)
                                          : GetColorU32(ImGuiCol_TextDisabled, 0.5f);
                if (IO.MouseClicked[0])
                    item->Show = !item->Show;
            } else {
                iconColor = item->Show ? GetColorU32(item->Color) : GPlt.Col_TxtDis;
            }
            DrawList.AddRectFilled(icon_bb.Min, icon_bb.Max, iconColor, 1);
            const char* label = GPlt.GetLegendLabel(i);
            const char* text_display_end = FindRenderedTextEnd(label, NULL);
            if (label != text_display_end)
                DrawList.AddText(legend_content_bb.Min + legend_padding + ImVec2(legend_icon_size, i * txt_ht), item->Show ? GPlt.Col_Txt : GPlt.Col_TxtDis, label, text_display_end);
        }
    }

    // render ticks
    if (HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &xt : GPlt.XTicks)
            DrawList.AddLine({xt.PixelPos, GPlt.BB_Grid.Max.y},{xt.PixelPos, GPlt.BB_Grid.Max.y - (xt.Major ? 10.0f : 5.0f)}, GPlt.Col_Border, 1);
    }
    if (HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &yt : GPlt.YTicks)
            DrawList.AddLine({GPlt.BB_Grid.Min.x, yt.PixelPos}, {GPlt.BB_Grid.Min.x + (yt.Major ? 10.0f : 5.0f), yt.PixelPos}, GPlt.Col_Border, 1);
    }

    // render selection
    if (plot.Selecting) {
        ImRect select_bb(ImMin(IO.MousePos, plot.SelectStart),
                         ImMax(IO.MousePos, plot.SelectStart));
        if (select_bb.GetWidth() > 2 && select_bb.GetHeight() > 2) {
            DrawList.AddRectFilled(select_bb.Min, select_bb.Max, GPlt.Col_SlctBg);
            DrawList.AddRect(select_bb.Min, select_bb.Max, GPlt.Col_SlctBd);
        }
    }

    // render crosshairs
    if (HasFlag(plot.Flags, ImPlotFlags_Crosshairs) && GPlt.Hov_Grid && GPlt.Hov_Frame &&
        !(plot.XAxis.Dragging || plot.YAxis.Dragging) && !plot.Selecting && !legend_hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        ImVec2 xy = IO.MousePos;
        ImVec2 h1(GPlt.BB_Grid.Min.x, xy.y);
        ImVec2 h2(xy.x - 5, xy.y);
        ImVec2 h3(xy.x + 5, xy.y);
        ImVec2 h4(GPlt.BB_Grid.Max.x, xy.y);
        ImVec2 v1(xy.x, GPlt.BB_Grid.Min.y);
        ImVec2 v2(xy.x, xy.y - 5);
        ImVec2 v3(xy.x, xy.y + 5);
        ImVec2 v4(xy.x, GPlt.BB_Grid.Max.y);
        DrawList.AddLine(h1, h2, GPlt.Col_Border);
        DrawList.AddLine(h3, h4, GPlt.Col_Border);
        DrawList.AddLine(v1, v2, GPlt.Col_Border);
        DrawList.AddLine(v3, v4, GPlt.Col_Border);
    }

    // render mouse pos
    if (HasFlag(plot.Flags, ImPlotFlags_MousePos) && GPlt.Hov_Grid) {
        static char buffer[128];
        sprintf(buffer, "%.2f,%.2f", GPlt.LastMousePos.x, GPlt.LastMousePos.y);
        ImVec2 size = CalcTextSize(buffer);
        ImVec2 pos  = GPlt.BB_Grid.Max - size - ImVec2(5, 5);
        DrawList.AddText(pos, GPlt.Col_Txt, buffer);
    }

    ImGui::PopClipRect();

    // render border
    DrawList.AddRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, GPlt.Col_Border);

    // FIT DATA --------------------------------------------------------------

    if (GPlt.FitThisFrame && GPlt.VisibleItemCount > 0) {
        if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMin))
            plot.XAxis.Min = GPlt.Extents.Min.x;
        if (!HasFlag(plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
            plot.XAxis.Max = GPlt.Extents.Max.x;
        if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
            plot.YAxis.Min = GPlt.Extents.Min.y;
        if (!HasFlag(plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
            plot.YAxis.Max = GPlt.Extents.Max.y;        
    }

    // CONTEXT MENU -----------------------------------------------------------

    if (HasFlag(plot.Flags, ImPlotFlags_ContextMenu) && GPlt.Hov_Frame && GPlt.Hov_Grid && IO.MouseDoubleClicked[1] && !legend_hovered)
        ImGui::OpenPopup("##Context");
    if (ImGui::BeginPopup("##Context")) {
        PlotContextMenu(plot);
        ImGui::EndPopup();
    }
    // CLEANUP ----------------------------------------------------------------

    // Reset legend items
    GPlt._LegendIndices.shrink(0);
    // Null current plot/data
    GPlt.CurrentPlot = NULL;
    GPlt.CurrentPlot = NULL;
    // Pop PushID at the end of BeginPlot
    PopID(); 
}

//=============================================================================
// STYLING
//=============================================================================

ImPlotStyle& GetPlotStyle() {
    return GPlt.Style;
}


void SetPlotSpec(ImPlotSpec spec) {
    GPlt.Style.Spec = spec;
}

void SetPlotLineStyle(float line_weight, const ImVec4& line_color) {
    GPlt.Style.LineWeight             = line_weight;
    GPlt.Style.Colors[ImPlotCol_Line] = line_color;
}

void SetPlotMarkerStyle(float marker_size, const ImVec4& fill_color) {
    GPlt.Style.MarkerSize = marker_size;
    GPlt.Style.Colors[ImPlotCol_Fill]= fill_color;
}

void SetPlotStyle(ImPlotSpec spec, float line_weight, const ImVec4& line_color, float marker_size, const ImVec4& fill_color) {
    GPlt.Style.Spec = spec;
    GPlt.Style.Colors[ImPlotCol_Line] = line_color;
    GPlt.Style.Colors[ImPlotCol_Fill]= fill_color;
    GPlt.Style.LineWeight             = line_weight;
    GPlt.Style.MarkerSize = marker_size;
}

//=============================================================================
// MARKERS
//=============================================================================

#define SQRT_1_2 0.70710678118f
#define SQRT_3_2 0.86602540378f

inline void TransformMarker(ImVec2* points, int n, const ImVec2& c, float s) {
    for (int i = 0; i < n; ++i) {
        points[i].x = c.x + points[i].x * s;
        points[i].y = c.y + points[i].y * s;
    }
}

inline void MarkerGeneral(ImDrawList& DrawList, ImVec2* points, int n, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    TransformMarker(points, n, c, s);
    if (fill)
        DrawList.AddConvexPolyFilled(points, n, col_fill);
    if (outline && !(fill && col_outline == col_fill)) {
        for (int i = 0; i < n; ++i)
            DrawList.AddLine(points[i], points[(i+1)%n], col_outline, weight);
    }
}

inline void MakerCircle(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[10] = {{1.0f, 0.0f},
                         {0.809017f, 0.58778524f},
                         {0.30901697f, 0.95105654f},
                         {-0.30901703f, 0.9510565f},
                         {-0.80901706f, 0.5877852f},
                         {-1.0f, 0.0f},
                         {-0.80901694f, -0.58778536f},
                         {-0.3090171f, -0.9510565f},
                         {0.30901712f, -0.9510565f},
                         {0.80901694f, -0.5877853f}};
    MarkerGeneral(DrawList, marker, 10, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerDiamond(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[4] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
    MarkerGeneral(DrawList, marker, 4, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerSquare(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[4] = {{SQRT_1_2,SQRT_1_2},{SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,SQRT_1_2}};
    MarkerGeneral(DrawList, marker, 4, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerUp(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[3] = {{SQRT_3_2,0.5f},{0,-1},{-SQRT_3_2,0.5f}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerDown(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[3] = {{SQRT_3_2,-0.5f},{0,1},{-SQRT_3_2,-0.5f}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerLeft(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[3] = {{-1,0}, {0.5, SQRT_3_2}, {0.5, -SQRT_3_2}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerRight(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[3] = {{1,0}, {-0.5, SQRT_3_2}, {-0.5, -SQRT_3_2}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill, weight);
}

inline void MarkerAsterisk(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[6] = {{SQRT_3_2, 0.5f}, {0, -1}, {-SQRT_3_2, 0.5f}, {SQRT_3_2, -0.5f}, {0, 1},  {-SQRT_3_2, -0.5f}};
    TransformMarker(marker, 6, c, s);
    DrawList.AddLine(marker[0], marker[5], col_outline, weight);
    DrawList.AddLine(marker[1], marker[4], col_outline, weight);
    DrawList.AddLine(marker[2], marker[3], col_outline, weight);
}

inline void MarkerPlus(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[4] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
    TransformMarker(marker, 4, c, s);
    DrawList.AddLine(marker[0], marker[2], col_outline, weight);
    DrawList.AddLine(marker[1], marker[3], col_outline, weight);
}

inline void MarkerCross(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill, float weight) {
    ImVec2 marker[4] = {{SQRT_1_2,SQRT_1_2},{SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,SQRT_1_2}};
    TransformMarker(marker, 4, c, s);
    DrawList.AddLine(marker[0], marker[2], col_outline, weight);
    DrawList.AddLine(marker[1], marker[3], col_outline, weight);
}

//=============================================================================
// PLOTTERS
//=============================================================================

struct ImPlotGetterData2D
{
    const float* Xs;
    const float* Ys;
    int Stride;
    ImPlotGetterData2D(const float* xs, const float* ys, int stride) {
        Xs = xs;
        Ys = ys;
        Stride = stride;
    }
};

inline float ImStrideIndex(const float* data, int idx, int stride) {
    return *(const float*)(const void*)((const unsigned char*)data + (size_t)idx * stride);
}

static ImVec2 ImPlotGetter2D(void* data, int idx) {
    ImPlotGetterData2D* data_2d = (ImPlotGetterData2D*)data;
    return { ImStrideIndex(data_2d->Xs, idx, data_2d->Stride), ImStrideIndex(data_2d->Ys, idx, data_2d->Stride) };
}

/// Main Plotting Function
void Plot(const char* label_id, const float* xs, const float* ys, int count, int offset, int stride) {
    ImPlotGetterData2D data(xs,ys,stride);
    Plot(label_id, &ImPlotGetter2D, (void*)&data, count, offset);
}

void Plot(const char* label_id, ImVec2 (*getter)(void* data, int idx), void* data, int count, int offset)
{
    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot != NULL, "Plot() Needs to be called between BeginPlot() and EndPlot()!");

    ImPlotItem* item = GPlt.RegisterItem(label_id);
    if (!item->Show)
        return;

    ImDrawList & DrawList = *ImGui::GetWindowDrawList();

    const bool rend_line = GPlt.Style.Colors[ImPlotCol_Line].w != 0 && GPlt.Style.LineWeight > 0;
    const bool rend_fill = GPlt.Style.Colors[ImPlotCol_Fill].w != 0;

    ImU32 col_line = GPlt.Style.Colors[ImPlotCol_Line].w == -1 ? GetColorU32(item->Color) : GetColorU32(GPlt.Style.Colors[ImPlotCol_Line]);
    ImU32 col_fill = GPlt.Style.Colors[ImPlotCol_Fill].w == -1 ? col_line                 : GetColorU32(GPlt.Style.Colors[ImPlotCol_Fill]);

    if (GPlt.Style.Colors[ImPlotCol_Line].w != -1)
        item->Color = GPlt.Style.Colors[ImPlotCol_Line];

    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    bool cull = HasFlag(GPlt.CurrentPlot->Flags, ImPlotFlags_CullData);

    // find data extents
    if (GPlt.FitThisFrame) {
        for (int i = 0; i < count; ++i) {
            ImVec2 p = getter(data, i);
            GPlt.Extents.Min.x = p.x < GPlt.Extents.Min.x ? p.x : GPlt.Extents.Min.x;
            GPlt.Extents.Max.x = p.x > GPlt.Extents.Max.x ? p.x : GPlt.Extents.Max.x;
            GPlt.Extents.Min.y = p.y < GPlt.Extents.Min.y ? p.y : GPlt.Extents.Min.y;
            GPlt.Extents.Max.y = p.y > GPlt.Extents.Max.y ? p.y : GPlt.Extents.Max.y;
        }
    }

    // render line segments
    if (count > 1 && rend_line && HasFlag(GPlt.Style.Spec, ImPlotSpec_SolidLine)) {
        const int    segments  = count - 1;
        int    i1 = offset;
        ImVec2 p1, p2;
        if (HasFlag(GPlt.CurrentPlot->Flags, ImPlotFlags_AntiAliased)) {
            for (int s = 0; s < segments; ++s) {
                const int i2 = (i1 + 1) % count;
                p1 = GPlt.ToPixels(getter(data, i1));
                p2 = GPlt.ToPixels(getter(data, i2));
                i1 = i2;
                if (!cull || GPlt.BB_Grid.Contains(p1) || GPlt.BB_Grid.Contains(p2))
                    DrawList.AddLine(p1, p2, col_line, GPlt.Style.LineWeight);
            }
        }
        else {
            const int    idx_count = segments * 6;
            const int    vtx_count = segments * 4;
            const ImVec2 uv        = DrawList._Data->TexUvWhitePixel;
            DrawList.PrimReserve(idx_count, vtx_count);
            int segments_culled = 0;
            for (int s = 0; s < segments; ++s) {
                const int i2 = (i1 + 1) % count;
                p1 = GPlt.ToPixels(getter(data, i1));
                p2 = GPlt.ToPixels(getter(data, i2));
                i1 = i2;
                if (!cull || GPlt.BB_Grid.Contains(p1) || GPlt.BB_Grid.Contains(p2)) {
                    float dx = p2.x - p1.x;
                    float dy = p2.y - p1.y;
                    IM_NORMALIZE2F_OVER_ZERO(dx, dy);
                    dx *= (GPlt.Style.LineWeight * 0.5f);
                    dy *= (GPlt.Style.LineWeight * 0.5f);                    
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
    }

    static ImPlotSpec AnyMakers = ImPlotSpec_Circle | ImPlotSpec_Square | ImPlotSpec_Diamond |
                                  ImPlotSpec_Up     | ImPlotSpec_Down   | ImPlotSpec_Left    | ImPlotSpec_Right | 
                                  ImPlotSpec_Cross  | ImPlotSpec_Plus   | ImPlotSpec_Asterisk;         

    // render markers
    if ((GPlt.Style.Spec & AnyMakers) != 0) {
        int idx = offset;
        for (int i = 0; i < count; ++i) {      
            ImVec2 c;
            c = GPlt.ToPixels(getter(data, idx));
            idx = (idx + 1) % count;
            if (!cull || GPlt.BB_Grid.Contains(c)) {
                // TODO: Optimize the loop and if statements, this is atrocious
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Circle)) 
                    MakerCircle(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);       
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Square))
                    MarkerSquare(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);     
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Diamond)) 
                    MarkerDiamond(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Up))
                    MarkerUp(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);     
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Down))    
                    MarkerDown(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);  
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Left))
                    MarkerLeft(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);     
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Right))    
                    MarkerRight(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);  
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Cross))
                    MarkerCross(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight); 
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Plus))
                    MarkerPlus(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight); 
                if (HasFlag(GPlt.Style.Spec, ImPlotSpec_Asterisk))
                    MarkerAsterisk(DrawList, c, GPlt.Style.MarkerSize, rend_line, col_line, rend_fill, col_fill, GPlt.Style.LineWeight);  
            }
        }    
    }
    ImGui::PopClipRect();
}

void PlotBar(const char* label_id, float width, const float* xs, const float* ys, int count, int offset, int stride) {
    ImPlotGetterData2D data(xs,ys,stride);
    PlotBar(label_id, width, &ImPlotGetter2D, (void*)&data, count, offset);
}

void PlotBar(const char* label_id, float width, ImVec2 (*getter)(void* data, int idx), void* data, int count, int offset) {

    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot != NULL, "PlotBar() Needs to be called between BeginPlot() and EndPlot()!");

    ImPlotItem* item = GPlt.RegisterItem(label_id);
    if (!item->Show)
        return;

    ImDrawList & DrawList = *ImGui::GetWindowDrawList();

    const bool rend_line = GPlt.Style.Colors[ImPlotCol_Line].w != 0 && GPlt.Style.LineWeight > 0;
    const bool rend_fill = GPlt.Style.Colors[ImPlotCol_Fill].w != 0;

    ImU32 col_line = GPlt.Style.Colors[ImPlotCol_Line].w == -1 ? GetColorU32(item->Color) : GetColorU32(GPlt.Style.Colors[ImPlotCol_Line]);
    ImU32 col_fill = GPlt.Style.Colors[ImPlotCol_Fill].w == -1 ? col_line                 : GetColorU32(GPlt.Style.Colors[ImPlotCol_Fill]);

    if (GPlt.Style.Colors[ImPlotCol_Line].w != -1)
        item->Color = GPlt.Style.Colors[ImPlotCol_Line];

    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    float half_width = width * 0.5f;

    // find data extents
    if (GPlt.FitThisFrame) {
        for (int i = 0; i < count; ++i) {
            ImVec2 p = getter(data, i);
            GPlt.Extents.Min.x = p.x - half_width < GPlt.Extents.Min.x ? p.x - half_width : GPlt.Extents.Min.x;
            GPlt.Extents.Max.x = p.x + half_width > GPlt.Extents.Max.x ? p.x + half_width : GPlt.Extents.Max.x;
            GPlt.Extents.Min.y = p.y < GPlt.Extents.Min.y ? p.y : GPlt.Extents.Min.y;
            GPlt.Extents.Max.y = p.y > GPlt.Extents.Max.y ? p.y : GPlt.Extents.Max.y;
        }
    }

    int idx = offset;
    for (int i = 0; i < count; ++i) {      
        ImVec2 p;
        p = getter(data, idx);
        idx = (idx + 1) % count;
        if (p.y == 0)
            continue;
        ImVec2 a = GPlt.ToPixels(p.x - half_width, p.y);
        ImVec2 b = GPlt.ToPixels(p.x + half_width, 0);
        if (rend_fill)
            DrawList.AddRectFilled(a, b, col_fill);
        if (rend_line)
            DrawList.AddRect(a, b, col_line);
    }
    ImGui::PopClipRect();
}



void PlotLabel(const char* text, float x, float y, const ImVec2& pixel_offset) {
    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot != NULL, "PlotLabel() Needs to be called between BeginPlot() and EndPlot()!");
    ImDrawList & DrawList = *ImGui::GetWindowDrawList();
    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);
    ImVec2 pos = GPlt.ToPixels({x,y}) + pixel_offset;
    DrawList.AddText(pos, GPlt.Col_Txt, text);
    ImGui::PopClipRect();
}


//=============================================================================
// UTIL
//=============================================================================

bool IsPlotHovered() { return GPlt.Hov_Grid; }
ImVec2 GetPlotMousePos() { return GPlt.LastMousePos; }

}  // namespace ImGui
