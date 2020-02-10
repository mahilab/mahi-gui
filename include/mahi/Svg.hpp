#pragma once
#include <nanovg.h>

class NSVGimage;

namespace mahi::gui {

void svgDraw(NVGcontext* vg, NSVGimage* svg);

} // mahi::gui