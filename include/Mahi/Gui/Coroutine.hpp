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

#include <Mahi/Util/Coroutine.hpp>
#include <imgui.h>

namespace mahi {
namespace gui {

/// mahi-gui specific coroutine yield instructions to supplement those already in
/// <Mahi/Util/Coroutine.hpp>

/// Yield instruction which waits until a key is pressed (use GLFW enums)
struct WaitForKeyPress : public util::YieldInstruction {
    WaitForKeyPress(int key) : m_key(key) { }
    bool is_over() override {
        return ImGui::IsKeyPressed(m_key);
    }
private:
    int m_key;
};

/// Yield instruction which waits until a mouse button is pressed (0=left, 1=right, 2=middle)
struct WaitForMouseClick : public util::YieldInstruction {
    WaitForMouseClick(int button) : m_button(button) { }
    bool is_over() override {
        return ImGui::IsMouseClicked(m_button);
    }
private:
    int m_button;
};

} // namespace gui
} // namespace mahi