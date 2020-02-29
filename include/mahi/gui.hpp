#pragma once

#include <mahi/gui/Application.hpp>
#include <mahi/gui/Color.hpp>
#include <mahi/gui/Macros.hpp>
#include <mahi/gui/Math.hpp>
#include <mahi/gui/Print.hpp>
#include <mahi/gui/Random.hpp>
#include <mahi/gui/Sequence.hpp>
#include <mahi/gui/System.hpp>
#include <mahi/gui/Tween.hpp>
#include <mahi/gui/Vec2.hpp>
#include <mahi/gui/Icons/IconsFontAwesome5.hpp>
#include <mahi/gui/Icons/IconsFontAwesome5Brands.hpp>
#include <mahi/gui/imgui_custom.hpp>
#include <mahi/gui/Svg.hpp>

#ifdef MAHI_GUI_COROUTINES
#include <mahi/gui/Coroutine.hpp>
#include <mahi/gui/Range.hpp>
#endif

// 3rd party libs
#include <nlohmann/json.hpp>
#include <nanosvg.h>

namespace mahi::gui {
    using json = nlohmann::json;
}