#pragma once
#include <nanovg.h>

struct NSVGimage;

namespace mahi::gui {

void svgDraw(NVGcontext* vg, NSVGimage* svg);

} // mahi::gui