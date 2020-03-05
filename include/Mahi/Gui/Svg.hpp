#pragma once
#include <nanovg.h>

struct NSVGimage;

namespace mahi {
namespace gui {

void svgDraw(NVGcontext* vg, NSVGimage* svg);

} // namespace gui
} // namespace mahi