/**
* NoesisUtils.h
* 06.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <NsDrawing/Point.h>

#include <vector>

namespace Noesis { class FrameworkElement; }


/**
* Remove margin offsets recursively */
NOESISGUI_API Noesis::Point SkipMargin(Noesis::Point pos, class Noesis::FrameworkElement* element);

/**
* Convert data to array */
NOESISGUI_API Noesis::ArrayRef<uint8_t> ToArray(const std::vector<char>& data);
