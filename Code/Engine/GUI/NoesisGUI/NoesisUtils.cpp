/**
* NoesisUtils.cpp
* 06.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/NoesisGUI/NoesisUtils.h"

#include <NsGui/FrameworkElement.h>
#include <NsDrawing/Thickness.h>
#include <NsGui/IView.h>


Noesis::Point SkipMargin(Noesis::Point pos, Noesis::FrameworkElement* element)
{
	Noesis::Point result(pos);
	if (!element) return result;

	if (auto root = element->GetView() ? element->GetView()->GetContent() : nullptr)
	{
		int depth = 100;
		auto margin = element->GetMargin();
		result.x -= margin.left;
		result.y -= margin.top;

		do
		{
			element = element->GetParent();
			auto margin = element->GetMargin();
			result.x -= margin.left;
			result.y -= margin.top;
		}
		while (--depth && root != element);
	}

	return result;
}

Noesis::ArrayRef<uint8_t> ToArray(const std::vector<char>& data)
{
	return Noesis::ArrayRef<uint8_t>((uint8_t*)data.data(), (uint32_t)data.size());
}
