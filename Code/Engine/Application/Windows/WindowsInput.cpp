/**
* WindowsInput.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsInput.h"


TInputSystemPtr GetWindowsInputSystem()
{
    return std::make_unique<LcInputSystemWindows>();
}
