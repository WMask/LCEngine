/**
* NoesisUtils.h
* 06.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <NsDrawing/Point.h>

namespace Noesis { class FrameworkElement; }


/**
* Remove margin offsets recursively */
NOESISGUI_API Noesis::Point SkipMargin(Noesis::Point pos, class Noesis::FrameworkElement* element);
