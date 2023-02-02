/**
* Module.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUI/GUIManager.h"

// copy NoesisGUI code to LCEngine/Code/Engine/GUI/NoesisGUI
#include <NsGui/XamlProvider.h>


#ifndef NOESISGUI_EXPORTS
#define NOESISGUI_API __declspec (dllimport)
#else
#define NOESISGUI_API __declspec (dllexport)
#endif


/**
* Noesis GUI manager */
class NOESISGUI_API INoesisGuiManager : public IGuiManager
{
public:
	/**
	* Set XAML provider */
	virtual void SetXamlProvider(Noesis::Ptr<Noesis::XamlProvider> provider) = 0;

};


NOESISGUI_API std::shared_ptr<INoesisGuiManager> GetGuiManager();
