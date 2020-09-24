// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <imgui.h>
#include <imgui_stdlib.h>
#include <vector>

// mahi-gui custom ImGui
namespace ImGui {

/// Enable Viewports
void EnableViewports();
/// Disable Viewports
void DisableViewports();
/// Enable Docking
void EnableDocking();
/// Disable Docking
void DisableDocking();
/// Begins a fixed size ImGui window (useful for single window apps)
bool BeginFixed(const char* name, const ImVec2& pos, const ImVec2& size, ImGuiWindowFlags flags = 0);
/// Shows a tooltip when an item is hovered for longer than delay.
void HoverTooltip(const char* tip, float delay = 1.0f);
/// Begins an disabled section. If disbled is true, everything until EndDisabled will be visible but disabled
void BeginDisabled(bool disabled = true, float alpha = 0.25f);
/// Ends a disabled section.
void EndDisabled();
/// A button that acts like a toggle, dimming itself when disabled.
bool ToggleButton(const char* label, bool* toggled, const ImVec2& size = ImVec2(0,0));
/// A regular button with a color.
bool ButtonColored(const char* label, const ImVec4& color, const ImVec2& size = ImVec2(0,0));
/// A series of mutually exclusive RadioButtons. Useful for mode selection.
bool ModeSelector(int* currentMode, const std::vector<std::string>& modes, bool horizontal = true);

bool DragDouble(const char* label, double* v, float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
bool DragDouble2(const char* label, double v[2], float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", float power = 1.0f);
bool DragDouble3(const char* label, double v[3], float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", float power = 1.0f);
bool DragDouble4(const char* label, double v[4], float v_speed = 1.0f, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", float power = 1.0f);
bool DragDoubleRange2(const char* label, double* v_current_min, double* v_current_max, float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);

bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
bool SliderDouble2(const char* label, double v[2], double v_min, double v_max, const char* format = "%.3f", float power = 1.0f);
bool SliderDouble3(const char* label, double v[3], double v_min, double v_max, const char* format = "%.3f", float power = 1.0f);
bool SliderDouble4(const char* label, double v[4], double v_min, double v_max, const char* format = "%.3f", float power = 1.0f);

bool InputDouble2(const char* label, double v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
bool InputDouble3(const char* label, double v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
bool InputDouble4(const char* label, double v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

void StyleColorsMahiDark1();
void StyleColorsMahiDark2();
void StyleColorsMahiDark3();
void StyleColorsMahiDark4();

} // namespace ImGui