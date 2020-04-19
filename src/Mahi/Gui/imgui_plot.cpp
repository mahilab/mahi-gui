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

namespace ImGui {

namespace {

//=============================================================================
// General Utils
//=============================================================================

/// Returns true if a flag is set
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
    f           = x / ImPow(10.0, (double)expv); /* between 1 and 10 */
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
    double PlotPos;
    float  PixelPos;
    bool   Major;
    ImVec2 Size;
    int    TextOffset;
};

struct ImPlotItem {
    ImPlotItem() {
        std::cout << "ImPlotItem()" << std::endl;
        Show = true;
        Color = NextColor();
        NameOffset = -1;
        Active = true;
        ID = 0;
    }
    ~ImPlotItem() { 
        ID = 0;
        std::cout << "~ImPlotItem()" << std::endl; 
    }
    bool Show;
    ImVec4 Color;
    int NameOffset;
    bool Active;
    ImGuiID ID;
};

/// Holds Plot state information that must persist for the lifetime of the plot
struct ImPlotData {
    ImPlotData() {
        std::cout << "ImPlotData()" << std::endl;
        DraggingX = DraggingY = Selecting = false;
        ZoomRateX = ZoomRateY = 0.1f;
        SelectStart = {0,0};
        Items.Reserve(10);
    }
    ~ImPlotData() { std::cout << "~ImPlotData()" << std::endl; }
    ImPool<ImPlotItem> Items;
    bool DraggingX;
    bool DraggingY;
    float ZoomRateX;
    float ZoomRateY;
    ImRect BB_Legend;
    bool Selecting;
    ImVec2 SelectStart;
};

/// Holds Plot state information that must persist only between calls to BeginPlot()/EndPlot()
struct ImPlotContext {
    ImPlotContext() {
        std::cout << "ImPlotContext()" << std::endl;
        CurrentPlot     = NULL;
        CurrentData = NULL;
        XTicks.reserve(50);
        YTicks.reserve(50);
        LegendItems.reserve(10);
    }
    ~ImPlotContext() { std::cout << "~ImPlotContext()" << std::endl; }
    /// PlotData    
    ImPool<ImPlotData> PlotData;
    /// Current Plot Data (Internal)
    ImPlotData* CurrentData;
    /// Current Plot (User Provided)
    ImPlot* CurrentPlot;
    /// Items to render in Legend
    ImVector<ImPlotItem*> LegendItems;
    ImGuiTextBuffer LegendNames;
    const char* GetItemName(const ImPlotItem* item) const {
        IM_ASSERT(item->NameOffset != -1 && item->NameOffset < LegendNames.Buf.Size);
        return LegendNames.Buf.Data + item->NameOffset;
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
    // Transformation
    ImRect XForm;
    float Mx, My;
        inline ImVec2 ToPixels(float x, float y) {
        ImVec2 out;
        out.x = XForm.Min.x + Mx * (x - CurrentPlot->XAxis.Min);
        out.y = XForm.Min.y + My * (y - CurrentPlot->YAxis.Min);
        return out;
    }
    // Culling
    ImRect CullRect;
    // Render flags
    bool RenderX, RenderY;
    // Mouse pos
    ImVec2 LastMousePos;
};

/// Global plot context
static ImPlotContext GPlt;

//=============================================================================
// Tick Utils
//=============================================================================

inline void GetTicks(float tMin, float tMax, int nMajor, int nMinor, ImVector<ImTick> &out) {
    out.shrink(0);
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

inline void LabelTicks(ImVector<ImTick> &ticks, ImGuiTextBuffer& buffer) {
    buffer.Buf.resize(0);
    char temp[16];
    for (auto &tk : ticks) {
        tk.TextOffset = buffer.size();
        sprintf(temp, "%g", tk.PlotPos);
        buffer.append(temp, temp + strlen(temp) + 1);
        tk.Size = CalcTextSize(buffer.Buf.Data + tk.TextOffset);
    }
}

inline void TransformTicks(ImVector<ImTick> &ticks, float tMin, float tMax, float pMin, float pMax) {
    const float m = (pMax - pMin) / (tMax - tMin);
    for (auto &tk : ticks)
        tk.PixelPos = (pMin + m * ((float)tk.PlotPos - tMin));  
}

} // private namespace

//=============================================================================
// BeginPlot()
//=============================================================================

bool BeginPlot(const char* title, ImPlot* plot_ptr, const char* x_label, const char* y_label, const ImVec2& size) {

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
    
    GPlt.CurrentPlot = plot_ptr;
    GPlt.CurrentData = GPlt.PlotData.GetOrAddByKey(ID);
    ImPlot &Plot = *plot_ptr;
    ImPlotData &Data = *GPlt.CurrentData;

    // Colors -----------------------------------------------------------------

    // get colors
    // GPlt.Col_Frame = plot.frame_color.w == -1 ? GetColorU32(ImGuiCol_FrameBg) : GetColorU32(plot.frame_color);
    // GPlt.Col_Bg = plot.background_color.w == -1 ? GetColorU32(ImGuiCol_WindowBg) : GetColorU32(plot.background_color);
    // GPlt.Col_Border = plot.border_color.w == -1 ? GetColorU32(ImGuiCol_Text, 0.5f) : GetColorU32(plot.border_color);

    GPlt.Col_Frame = GetColorU32(ImGuiCol_FrameBg);
    GPlt.Col_Bg = GetColorU32(ImGuiCol_WindowBg);
    GPlt.Col_Border = GetColorU32(ImGuiCol_Text, 0.5f);

    const ImVec4 col_xAxis = Plot.XAxis.Col.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : Plot.XAxis.Col;
    GPlt.Col_XMajor   = GetColorU32(col_xAxis);
    GPlt.Col_XMinor   = GetColorU32(col_xAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.Col_XTxt = GetColorU32({col_xAxis.x, col_xAxis.y, col_xAxis.z, 1});

    const ImVec4 col_yAxis = Plot.YAxis.Col.w == -1 ? ImGui::GetStyle().Colors[ImGuiCol_Text] * ImVec4(1, 1, 1, 0.25f) : Plot.YAxis.Col;
    GPlt.Col_YMajor   = GetColorU32(col_yAxis);
    GPlt.Col_YMinor   = GetColorU32(col_yAxis * ImVec4(1, 1, 1, 0.25f));
    GPlt.Col_YTxt = GetColorU32({col_yAxis.x, col_yAxis.y, col_yAxis.z, 1});

    GPlt.Col_Txt     = GetColorU32(ImGuiCol_Text);
    GPlt.Col_TxtDis = GetColorU32(ImGuiCol_TextDisabled);
    GPlt.Col_SlctBg  = GetColorU32({.118f, .565f, 1, 0.25f});
    GPlt.Col_SlctBd =  GetColorU32({.118f, .565f, 1, 1.00f});

    // BB AND HOVER -----------------------------------------------------------

    // frame
    const ImVec2 frame_size = CalcItemSize(size, 100, 100);
    GPlt.BB_Frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ItemSize(GPlt.BB_Frame);
    if (!ItemAdd(GPlt.BB_Frame, 0, &GPlt.BB_Frame)) {
        GPlt.CurrentPlot = NULL;
        GPlt.CurrentData = NULL;
        return false;
    }
    GPlt.Hov_Frame = ItemHoverable(GPlt.BB_Frame, ID);
    RenderFrame(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, GPlt.Col_Frame, true, Style.FrameRounding);

    // canvas bb
    GPlt.BB_Canvas = ImRect(GPlt.BB_Frame.Min + Style.WindowPadding, GPlt.BB_Frame.Max - Style.WindowPadding);

    // constrain axes
    if (Plot.XAxis.Max <= Plot.XAxis.Min)
        Plot.XAxis.Max = Plot.XAxis.Min + FLT_EPSILON;
    if (Plot.YAxis.Max <= Plot.YAxis.Min)
        Plot.YAxis.Max = Plot.YAxis.Min + FLT_EPSILON;

    // adaptive divisions
    if (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        Plot.XAxis.Divisions = (int)std::round(0.003 * GPlt.BB_Canvas.GetWidth());
        if (Plot.XAxis.Divisions < 2)
            Plot.XAxis.Divisions = 2;
    }
    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Adaptive)) {
        Plot.YAxis.Divisions = (int)std::round(0.003 * GPlt.BB_Canvas.GetHeight());
        if (Plot.YAxis.Divisions < 2)
            Plot.YAxis.Divisions = 2;
    }

    GPlt.RenderX = (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_GridLines) || 
                    Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickMarks) || 
                    Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  Plot.XAxis.Divisions > 1;
    GPlt.RenderY = (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_GridLines) || 
                    Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickMarks) || 
                    Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) &&  Plot.YAxis.Divisions > 1;

    // get ticks
    if (GPlt.RenderX)
        GetTicks(Plot.XAxis.Min, Plot.XAxis.Max, Plot.XAxis.Divisions, Plot.XAxis.Subdivisions, GPlt.XTicks);
    if (GPlt.RenderY)
        GetTicks(Plot.YAxis.Min, Plot.YAxis.Max, Plot.YAxis.Divisions, Plot.YAxis.Subdivisions, GPlt.YTicks);

    // label ticks
    if (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickLabels))
        LabelTicks(GPlt.XTicks, GPlt.XTickLabels);
    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickLabels))
        LabelTicks(GPlt.YTicks, GPlt.YTickLabels);

    // get max y-tick width
    float maxLabelWidth = 0;
    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        for (auto &yt : GPlt.YTicks)
            maxLabelWidth = yt.Size.x > maxLabelWidth ? yt.Size.x : maxLabelWidth;
    }

    // grid bb
    const ImVec2 title_size = CalcTextSize(title, NULL, true);
    const float txt_off    = 5;
    const float txt_height = GetTextLineHeight();
    const float pad_top    = title_size.x > 0.0f ? txt_height + txt_off : 0;
    const float pad_bot    = (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickLabels) ? txt_height + txt_off : 0) + (x_label ? txt_height + txt_off : 0);
    const float pad_left   = (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickLabels) ? maxLabelWidth + txt_off : 0) + (y_label ? txt_height + txt_off : 0);
    GPlt.BB_Grid           = ImRect(GPlt.BB_Canvas.Min + ImVec2(pad_left, pad_top), GPlt.BB_Canvas.Max - ImVec2(0, pad_bot));
    GPlt.Hov_Grid          = GPlt.BB_Grid.Contains(IO.MousePos);

    // axis region bbs
    const ImRect xAxisRegion_bb(GPlt.BB_Grid.Min + ImVec2(10, 0), {GPlt.BB_Grid.Max.x, GPlt.BB_Frame.Max.y});
    const bool   xAxisRegion_hovered = xAxisRegion_bb.Contains(IO.MousePos);
    const ImRect yAxisRegion_bb({GPlt.BB_Frame.Min.x, GPlt.BB_Grid.Min.y}, GPlt.BB_Grid.Max - ImVec2(0, 10));
    const bool   yAxisRegion_hovered = yAxisRegion_bb.Contains(IO.MousePos);

    const bool legend_hovered = Flagged(Plot.Flags, ImPlotFlags_Legend) ? GPlt.Hov_Frame && Data.BB_Legend.Contains(IO.MousePos) : false;

    // DRAG INPUT -------------------------------------------------------------

    // end drags
    if (Data.DraggingX && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        Data.DraggingX           = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0;
    }
    if (Data.DraggingY && (IO.MouseReleased[0] || !IO.MouseDown[0])) {
        Data.DraggingY           = false;
        G.IO.MouseDragMaxDistanceSqr[0] = 0;
    }
    // do drag
    if (Data.DraggingX || Data.DraggingY) {
        bool xLocked = Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMin) || 
                       Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMax);
        if (!xLocked && Data.DraggingX) {
            float dir  = Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? -1.0f : 1.0f;
            float delX = dir * IO.MouseDelta.x * (Plot.XAxis.Max - Plot.XAxis.Min) / (GPlt.BB_Grid.Max.x - GPlt.BB_Grid.Min.x);
            if (!Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.XAxis.Min -= delX;
            if (!Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.XAxis.Max -= delX;
        }
        bool yLocked = Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin) || 
                       Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMax);
        if (!yLocked && Data.DraggingY) {
            float dir  = Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? -1.0f : 1.0f;
            float delY = dir * IO.MouseDelta.y * (Plot.YAxis.Max - Plot.YAxis.Min) / (GPlt.BB_Grid.Max.y - GPlt.BB_Grid.Min.y);
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.YAxis.Min += delY;
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.YAxis.Max += delY;
        }
        if ((xLocked && yLocked) || (xLocked && Data.DraggingX && !Data.DraggingY) ||
            (yLocked && Data.DraggingY && !Data.DraggingX))
            ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
        else if (xLocked || (!Data.DraggingX && Data.DraggingY))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        else if (yLocked || (!Data.DraggingY && Data.DraggingX))
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    // start drag
    if (GPlt.Hov_Frame && xAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !Data.Selecting && !legend_hovered)
        Data.DraggingX = true;
    if (GPlt.Hov_Frame && yAxisRegion_hovered && IO.MouseDragMaxDistanceSqr[0] > 5 && !Data.Selecting && !legend_hovered)
        Data.DraggingY = true;


    // SCROLL INPUT -----------------------------------------------------------

    if (GPlt.Hov_Frame && (xAxisRegion_hovered || yAxisRegion_hovered) && IO.MouseWheel != 0) {
        float xRange    = Plot.XAxis.Max - Plot.XAxis.Min;
        float yRange    = Plot.YAxis.Max - Plot.YAxis.Min;
        float xZoomRate = Data.ZoomRateX;
        float yZoomRate = Data.ZoomRateY;
        if (IO.MouseWheel > 0) {
            xZoomRate = (-xZoomRate) / (1.0f + (2.0f * xZoomRate));
            yZoomRate = (-yZoomRate) / (1.0f + (2.0f * yZoomRate));
        }
        // Determine center of zoom
        float xCenter, yCenter;
        if (xAxisRegion_hovered && yAxisRegion_hovered) {
            // Mouse pointer is in plot -> zoom in on mouse pointer
            const ImRect pixZoom(Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Max.x : GPlt.BB_Grid.Min.x,
                                 Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Min.y : GPlt.BB_Grid.Max.y,
                                 Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Min.x : GPlt.BB_Grid.Max.x,
                                 Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Max.y : GPlt.BB_Grid.Min.y);
            xCenter = Remap(IO.MousePos.x, pixZoom.Min.x, pixZoom.Max.x, Plot.XAxis.Min, Plot.XAxis.Max);
            yCenter = Remap(IO.MousePos.y, pixZoom.Min.y, pixZoom.Max.y, Plot.YAxis.Min, Plot.YAxis.Max);
        } else {
            // Zoom in on center of plot
            xCenter = 0.5f * (Plot.XAxis.Max + Plot.XAxis.Min);
            yCenter = 0.5f * (Plot.YAxis.Max + Plot.YAxis.Min);
        }

        if (xAxisRegion_hovered && yAxisRegion_hovered) {
            // x axis
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin) && !Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMax)) {
                // Zoom in on center
                Plot.XAxis.Min =
                    xCenter + ((Plot.XAxis.Min - xCenter) * (1.0f + (2.0f * xZoomRate)));
                Plot.XAxis.Max =
                    xCenter + ((Plot.XAxis.Max - xCenter) * (1.0f + (2.0f * xZoomRate)));
            } else if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.XAxis.Min -= xZoomRate * xRange;
            else if (!Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.XAxis.Max += xZoomRate * xRange;

            // y axis
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin) && !Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMax)) {
                // Zoom in on center
                Plot.YAxis.Min =
                    yCenter + (1.0f + (2.0f * yZoomRate)) * (Plot.YAxis.Min - yCenter);
                Plot.YAxis.Max =
                    yCenter + (1.0f + (2.0f * yZoomRate)) * (Plot.YAxis.Max - yCenter);
            } else if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.YAxis.Min -= yZoomRate * yRange;
            else if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.YAxis.Max += yZoomRate * yRange;
        } else if (xAxisRegion_hovered) {
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.XAxis.Min -= xZoomRate * xRange;
            if (!Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.XAxis.Max += xZoomRate * xRange;
        } else {
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMin))
                Plot.YAxis.Min -= yZoomRate * yRange;
            if (!Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_LockMax))
                Plot.YAxis.Max += yZoomRate * yRange;
        }
    }

    // TRANSFORMATION ---------------------------------------------------------

    // get pixels for transforms
    GPlt.XForm = ImRect(Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Max.x : GPlt.BB_Grid.Min.x,
                        Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Min.y : GPlt.BB_Grid.Max.y,
                        Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Min.x : GPlt.BB_Grid.Max.x,
                        Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_Flip) ? GPlt.BB_Grid.Max.y : GPlt.BB_Grid.Min.y);
    
    // set mouse position
    GPlt.LastMousePos.x = Remap(IO.MousePos.x, GPlt.XForm.Min.x, GPlt.XForm.Max.x, Plot.XAxis.Min, Plot.XAxis.Max);
    GPlt.LastMousePos.y = Remap(IO.MousePos.y, GPlt.XForm.Min.y, GPlt.XForm.Max.y, Plot.YAxis.Min, Plot.YAxis.Max);

    // SELECTION --------------------------------------------------------------

    // confirm selection
    if (Data.Selecting && (IO.MouseReleased[1] || !IO.MouseDown[1])) {
        ImVec2 slcSize = Data.SelectStart - IO.MousePos;
        if (std::abs(slcSize.x) > 2 && std::abs(slcSize.y) > 2) {
            ImVec2 p1, p2;
            p1.x = Remap(Data.SelectStart.x, GPlt.XForm.Min.x, GPlt.XForm.Max.x, Plot.XAxis.Min, Plot.XAxis.Max);
            p1.y = Remap(Data.SelectStart.y, GPlt.XForm.Min.y, GPlt.XForm.Max.y, Plot.YAxis.Min, Plot.YAxis.Max);
            p2.x = Remap(IO.MousePos.x, GPlt.XForm.Min.x, GPlt.XForm.Max.x, Plot.XAxis.Min, Plot.XAxis.Max);
            p2.y = Remap(IO.MousePos.y, GPlt.XForm.Min.y, GPlt.XForm.Max.y, Plot.YAxis.Min, Plot.YAxis.Max);
            Plot.XAxis.Min = ImMin(p1.x, p2.x);
            Plot.XAxis.Max = ImMax(p1.x, p2.x);
            Plot.YAxis.Min = ImMin(p1.y, p2.y);
            Plot.YAxis.Max = ImMax(p1.y, p2.y);
        }
        Data.Selecting = false;
    }
    // cancel selection
    if (Data.Selecting && (IO.MouseClicked[0] || IO.MouseDown[0])) {
        Data.Selecting = false;
    }
    // begin selection
    if (GPlt.Hov_Frame && GPlt.Hov_Grid && IO.MouseClicked[1] && Flagged(Plot.Flags, ImPlotFlags_Selection)) {
        Data.SelectStart = IO.MousePos;
        Data.Selecting    = true;
    }

    // FOCUS ------------------------------------------------------------------

    // focus window
    if ((IO.MouseClicked[0] || IO.MouseClicked[1]) && GPlt.Hov_Frame)
        FocusWindow(GetCurrentWindow());
            
    // RENDER -----------------------------------------------------------------

    GPlt.Mx        = (GPlt.XForm.Max.x - GPlt.XForm.Min.x) / (Plot.XAxis.Max - Plot.XAxis.Min);
    GPlt.My        = (GPlt.XForm.Max.y - GPlt.XForm.Min.y) / (Plot.YAxis.Max - Plot.YAxis.Min);
    GPlt.CullRect  = ImRect(ImMin(GPlt.XForm.Min.x, GPlt.XForm.Max.x), ImMin(GPlt.XForm.Min.y, GPlt.XForm.Max.y), 
                            ImMax(GPlt.XForm.Min.x, GPlt.XForm.Max.x), ImMax(GPlt.XForm.Min.y, GPlt.XForm.Max.y));

    // grid bg
    DrawList.AddRectFilled(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, GPlt.Col_Bg);

    // render axes
    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    // transform ticks
    if (GPlt.RenderX)
        TransformTicks(GPlt.XTicks, Plot.XAxis.Min, Plot.XAxis.Max, GPlt.XForm.Min.x, GPlt.XForm.Max.x);
    if (GPlt.RenderY)
        TransformTicks(GPlt.YTicks, Plot.YAxis.Min, Plot.YAxis.Max, GPlt.XForm.Min.y, GPlt.XForm.Max.y);

    // render grid
    if (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_GridLines)) {
        for (auto &xt : GPlt.XTicks)
            DrawList.AddLine({xt.PixelPos, GPlt.BB_Grid.Min.y}, {xt.PixelPos, GPlt.BB_Grid.Max.y}, xt.Major ? GPlt.Col_XMajor : GPlt.Col_XMinor, 1);
    }

    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_GridLines)) {
        for (auto &yt : GPlt.YTicks)
            DrawList.AddLine({GPlt.BB_Grid.Min.x, yt.PixelPos}, {GPlt.BB_Grid.Max.x, yt.PixelPos}, yt.Major ? GPlt.Col_YMajor : GPlt.Col_YMinor, 1);
    }

    ImGui::PopClipRect();

    // render title
    if (title_size.x > 0.0f) {
        RenderText(ImVec2(GPlt.BB_Canvas.GetCenter().x - title_size.x * 0.5f, GPlt.BB_Canvas.Min.y), title, NULL, true);
    }

    // render labels
    if (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, true);
        for (auto &xt : GPlt.XTicks) {
            if (xt.PixelPos >= GPlt.BB_Grid.Min.x && xt.PixelPos <= GPlt.BB_Grid.Max.x)
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
    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickLabels)) {
        ImGui::PushClipRect(GPlt.BB_Frame.Min, GPlt.BB_Frame.Max, true);
        for (auto &yt : GPlt.YTicks) {
            if (yt.PixelPos >= GPlt.BB_Grid.Min.y && yt.PixelPos <= GPlt.BB_Grid.Max.y)
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
    for (int i = 0; i < Data.Items.GetSize(); ++i) 
        Data.Items.GetByIndex(i)->Active = false;    
    // clear item names
    GPlt.LegendNames.Buf.resize(0);
    return true;
}

//=============================================================================
// EndPlot()
//=============================================================================

void EndPlot() {
    IM_ASSERT_USER_ERROR(GPlt.Plot != NULL, "Mismatched BeginPlot()/EndPlot()!");
    ImPlot &Plot = *GPlt.CurrentPlot;
    ImPlotData &Data = *GPlt.CurrentData;

    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    ImDrawList & DrawList = *Window->DrawList;
    const ImGuiIO &   IO = GetIO();

    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    // render legend
    const float textHeight = GetTextLineHeight();
    const ImVec2 legend_offset(10, 10);
    const ImVec2 legend_padding(5, 5);
    const float  legend_icon_size = textHeight;
    ImRect legend_content_bb;
    int nItems = GPlt.LegendItems.size();
    bool legend_hovered = false;
    if (Flagged(Plot.Flags, ImPlotFlags_Legend) && nItems > 0) {
        // get max width
        float maxLabelWidth = 0;
        for (int i = 0; i < nItems; ++i) {
            const char* label = GPlt.GetItemName(GPlt.LegendItems[i]);
            auto labelWidth = CalcTextSize(label, NULL, true);
            maxLabelWidth   = labelWidth.x > maxLabelWidth ? labelWidth.x : maxLabelWidth;
        }
        legend_content_bb = ImRect(GPlt.BB_Grid.Min + legend_offset, GPlt.BB_Grid.Min + legend_offset + ImVec2(maxLabelWidth, nItems * textHeight));
        Data.BB_Legend    = ImRect(legend_content_bb.Min, legend_content_bb.Max + legend_padding * 2 + ImVec2(legend_icon_size, 0));
        legend_hovered = Flagged(Plot.Flags, ImPlotFlags_Legend) ? GPlt.Hov_Frame && Data.BB_Legend.Contains(IO.MousePos) : false;
        // render legend box
        DrawList.AddRectFilled(Data.BB_Legend.Min, Data.BB_Legend.Max, GetColorU32(ImGuiCol_PopupBg));
        DrawList.AddRect(Data.BB_Legend.Min, Data.BB_Legend.Max, GPlt.Col_Border);
        // render each legend item
        for (int i = 0; i < nItems; ++i) {
            ImPlotItem* item = GPlt.LegendItems[i];
            ImRect icon_bb;
            icon_bb.Min = legend_content_bb.Min + legend_padding + ImVec2(0, i * textHeight) + ImVec2(2, 2);
            icon_bb.Max = legend_content_bb.Min + legend_padding + ImVec2(0, i * textHeight) + ImVec2(legend_icon_size - 2, legend_icon_size - 2);
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
            const char* label = GPlt.GetItemName(item);
            const char* text_display_end = FindRenderedTextEnd(label, NULL);
            if (label != text_display_end)
                DrawList.AddText(legend_content_bb.Min + legend_padding + ImVec2(legend_icon_size, i * textHeight), item->Show ? GPlt.Col_Txt : GPlt.Col_TxtDis, label, text_display_end);
        }
    }

    // render ticks
    if (Flagged(Plot.XAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &xt : GPlt.XTicks)
            DrawList.AddLine({xt.PixelPos, GPlt.BB_Grid.Max.y},{xt.PixelPos, GPlt.BB_Grid.Max.y - (xt.Major ? 10.0f : 5.0f)}, GPlt.Col_Border, 1);
    }
    if (Flagged(Plot.YAxis.Flags, ImPlotAxisFlags_TickMarks)) {
        for (auto &yt : GPlt.YTicks)
            DrawList.AddLine({GPlt.BB_Grid.Min.x, yt.PixelPos}, {GPlt.BB_Grid.Min.x + (yt.Major ? 10.0f : 5.0f), yt.PixelPos}, GPlt.Col_Border, 1);
    }

    // render selection
    if (Data.Selecting) {
        ImRect select_bb(ImMin(IO.MousePos, Data.SelectStart),
                         ImMax(IO.MousePos, Data.SelectStart));
        if (select_bb.GetWidth() > 2 && select_bb.GetHeight() > 2) {
            DrawList.AddRectFilled(select_bb.Min, select_bb.Max, GPlt.Col_SlctBg);
            DrawList.AddRect(select_bb.Min, select_bb.Max, GPlt.Col_SlctBd);
        }
    }

    // render crosshairs
    if (Flagged(Plot.Flags, ImPlotFlags_Crosshairs) && GPlt.Hov_Grid && GPlt.Hov_Frame &&
        !(Data.DraggingX || Data.DraggingY) && !Data.Selecting && !legend_hovered) {
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
    if (Flagged(Plot.Flags, ImPlotFlags_MousePos) && GPlt.Hov_Grid) {
        static char buffer[64];
        sprintf(buffer, "%.2f,%.2f", GPlt.LastMousePos.x, GPlt.LastMousePos.y);
        ImVec2 size = CalcTextSize(buffer);
        ImVec2 pos  = GPlt.BB_Grid.Max - size - ImVec2(5, 5);
        DrawList.AddText(pos, GPlt.Col_Txt, buffer);
    }

    ImGui::PopClipRect();

    // render border
    DrawList.AddRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, GPlt.Col_Border);

    // CLEANUP ----------------------------------------------------------------

    // Reset legend items
    GPlt.LegendItems.shrink(0);
    // Null current plot/data
    GPlt.CurrentPlot = NULL;
    GPlt.CurrentData = NULL;
    // Pop PushID at the end of BeginPlot
    PopID(); 
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

inline void MarkerGeneral(ImDrawList& DrawList, ImVec2* points, int n, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    TransformMarker(points, n, c, s);
    if (fill)
        DrawList.AddConvexPolyFilled(points, n, col_fill);
    if (outline && !(fill && col_outline == col_fill)) {
        for (int i = 0; i < n; ++i)
            DrawList.AddLine(points[i], points[(i+1)%n], col_outline, 1);
    }
}

inline void MakerCircle(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
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
    MarkerGeneral(DrawList, marker, 10, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerDiamond(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[4] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
    MarkerGeneral(DrawList, marker, 4, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerSquare(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[4] = {{SQRT_1_2,SQRT_1_2},{SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,SQRT_1_2}};
    MarkerGeneral(DrawList, marker, 4, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerUp(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[3] = {{SQRT_3_2,0.5f},{0,-1},{-SQRT_3_2,0.5f}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerDown(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[3] = {{SQRT_3_2,-0.5f},{0,1},{-SQRT_3_2,-0.5f}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerLeft(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[3] = {{-1,0}, {0.5, SQRT_3_2}, {0.5, -SQRT_3_2}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerRight(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[3] = {{1,0}, {-0.5, SQRT_3_2}, {-0.5, -SQRT_3_2}};
    MarkerGeneral(DrawList, marker, 3, c, s, outline, col_outline, fill, col_fill);
}

inline void MarkerAsterisk(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[6] = {{SQRT_3_2, 0.5f},  {0, -1}, {-SQRT_3_2, 0.5f},
                        {SQRT_3_2, -0.5f}, {0, 1},  {-SQRT_3_2, -0.5f}};
    TransformMarker(marker, 6, c, s);
    DrawList.AddLine(marker[0], marker[5], col_outline, 1);
    DrawList.AddLine(marker[1], marker[4], col_outline, 1);
    DrawList.AddLine(marker[2], marker[3], col_outline, 1);
}

inline void MarkerPlus(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[4] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
    TransformMarker(marker, 4, c, s);
    DrawList.AddLine(marker[0], marker[2], col_outline, 1);
    DrawList.AddLine(marker[1], marker[3], col_outline, 1);
}

inline void MarkerCross(ImDrawList& DrawList, const ImVec2& c, float s, bool outline, ImU32 col_outline, bool fill, ImU32 col_fill) {
    ImVec2 marker[4] = {{SQRT_1_2,SQRT_1_2},{SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,-SQRT_1_2},{-SQRT_1_2,SQRT_1_2}};
    TransformMarker(marker, 4, c, s);
    DrawList.AddLine(marker[0], marker[2], col_outline, 1);
    DrawList.AddLine(marker[1], marker[3], col_outline, 1);
}

//=============================================================================
// PLOTTERS
//=============================================================================

/// Main Plotting Function
void PlotLine(const char* label, const float* xs, const float* ys, int size, 
              ImPlotSpec spec, const ImVec4& color_line, const ImVec4& color_fill, 
              int offset, int stride, bool cull)
{
    IM_ASSERT_USER_ERROR(GPlt.CurrentPlot != NULL, "PlotLine() Needs to be called between BeginPlot() and EndPlot()!");
 
    ImGuiID id = GetID(label);
    ImPlotItem* item = GPlt.CurrentData->Items.GetOrAddByKey(id);
    item->Active = true;
    item->ID = id;
    GPlt.LegendItems.push_back(item);
    item->NameOffset = GPlt.LegendNames.size();
    GPlt.LegendNames.append(label, label + strlen(label) + 1);

    if (!item->Show)
        return;

    ImDrawList & DrawList = *ImGui::GetWindowDrawList();

    const bool rend_line = color_line.w != 0;
    const bool rend_fill = color_fill.w != 0;

    ImU32 col_line = color_line.w == -1 ? GetColorU32(item->Color) : GetColorU32(color_line);
    ImU32 col_fill = color_fill.w == -1 ? col_line    : GetColorU32(color_fill);

    ImGui::PushClipRect(GPlt.BB_Grid.Min, GPlt.BB_Grid.Max, true);

    // render line segments
    if (size > 1 && rend_line && Flagged(spec, ImPlotSpec_SolidLine)) {
        const float  thickness = 1.0f;
        const int    segments  = size - 1;
        int    i1 = offset;
        ImVec2 p1, p2;
        if (Flagged(spec, ImPlotSpec_AntiAliased)) {
            for (int s = 0; s < segments; ++s) {
                const int i2 = i1 + 1 == size ? 0 : i1 + 1;
                p1 = GPlt.ToPixels(xs[i1], ys[i1]);
                p2 = GPlt.ToPixels(xs[i2], ys[i2]);
                i1 = i2;
                if (!cull || GPlt.CullRect.Contains(p1) || GPlt.CullRect.Contains(p2))
                    DrawList.AddLine(p1, p2, col_line, thickness);
            }
        }
        else {
            const int    idx_count = segments * 6;
            const int    vtx_count = segments * 4;
            const ImVec2 uv        = DrawList._Data->TexUvWhitePixel;
            DrawList.PrimReserve(idx_count, vtx_count);
            int segments_culled = 0;
            for (int s = 0; s < segments; ++s) {
                const int i2 = i1 + 1 == size ? 0 : i1 + 1;
                p1 = GPlt.ToPixels(xs[i1], ys[i1]);
                p2 = GPlt.ToPixels(xs[i2], ys[i2]);
                i1 = i2;
                if (!cull || GPlt.CullRect.Contains(p1) || GPlt.CullRect.Contains(p2)) {
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
    }

    // render markers
    bool aa_line_bak = ImGui::GetStyle().AntiAliasedLines;
    bool aa_fill_bak = ImGui::GetStyle().AntiAliasedFill;
    ImGui::GetStyle().AntiAliasedLines = Flagged(spec, ImPlotSpec_AntiAliased);
    ImGui::GetStyle().AntiAliasedFill  = Flagged(spec, ImPlotSpec_AntiAliased);
    for (int i = 0; i < size; ++i) {
        ImVec2 c;
        c = GPlt.ToPixels(xs[i], ys[i]);
        if (GPlt.CullRect.Contains(c)) {
            if (Flagged(spec, ImPlotSpec_Circle)) 
                MakerCircle(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);            
            if (Flagged(spec, ImPlotSpec_Diamond)) 
                MarkerDiamond(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);
            if (Flagged(spec, ImPlotSpec_Square))
                MarkerSquare(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);
            if (Flagged(spec, ImPlotSpec_Up))
                MarkerUp(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);     
            if (Flagged(spec, ImPlotSpec_Down))    
                MarkerDown(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);  
            if (Flagged(spec, ImPlotSpec_Left))
                MarkerLeft(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);     
            if (Flagged(spec, ImPlotSpec_Right))    
                MarkerRight(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);  
            if (Flagged(spec, ImPlotSpec_Asterisk))
                MarkerAsterisk(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill);  
            if (Flagged(spec, ImPlotSpec_Plus))
                MarkerPlus(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill); 
            if (Flagged(spec, ImPlotSpec_Cross))
                MarkerCross(DrawList, c, 5, rend_line, col_line, rend_fill, col_fill); 

        }
    }    
    ImGui::GetStyle().AntiAliasedLines = aa_line_bak;
    ImGui::GetStyle().AntiAliasedFill  = aa_fill_bak;

    ImGui::PopClipRect();
}



}  // namespace ImGui
