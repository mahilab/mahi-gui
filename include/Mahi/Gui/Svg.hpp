#pragma once
#include <nanovg.h>

struct NSVGimage;

namespace mahi {
namespace gui {

void svg_draw(NVGcontext* vg, NSVGimage* svg);

} // namespace gui
} // namespace mahi