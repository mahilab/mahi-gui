#pragma once

/// Macro to request high performance GPU in systems (usually laptops) with both
/// dedicated and discrete GPUs
#if defined(_WIN32) && defined(MAHUI_USE_DISCRETE_GPU)
    extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
#endif

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(MAHUI_NO_CONSOLE)
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif