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

/// Macro to request high performance GPU in systems (usually laptops) with both
/// dedicated and discrete GPUs
#if defined(_WIN32) && defined(MAHI_GUI_USE_DISCRETE_GPU)
    extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
#endif

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(MAHI_GUI_NO_CONSOLE)
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif