/**
* Application.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Application.h"
#include "Application/Windows/WindowsApplication.h"


std::shared_ptr<IApplication> IApplication::GetPlatformApp()
{
#ifdef _WINDOWS
	return std::shared_ptr<IApplication>(new LCWindowsApplication());
#endif

	return std::shared_ptr<IApplication>();
}

IApplication::IApplication()
{
}

IApplication::~IApplication()
{
}

IApplication::IApplication(const IApplication&)
{
}

IApplication& IApplication::operator=(const IApplication&)
{
	return *this;
}
