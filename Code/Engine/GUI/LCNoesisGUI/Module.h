/**
* Module.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUI/GUIManager.h"
#include "World/Module.h"

#include <NsCore/Ptr.h>


#ifndef NOESISGUI_EXPORTS
#define NOESISGUI_API __declspec (dllimport)
#else
#define NOESISGUI_API __declspec (dllexport)
#endif


namespace Noesis { class XamlProvider; }


/**
* Noesis GUI manager */
class NOESISGUI_API INoesisGuiManager : public IGuiManager
{
public:
	/**
	* Initialize Noesis */
	virtual void NoesisInit(Noesis::Ptr<class Noesis::XamlProvider> provider, const std::string& inResources) = 0;

};

/**
* Noesis GUI manager pointer */
typedef std::shared_ptr<INoesisGuiManager> INoesisGuiManagerPtr;

/**
* Get Noesis GUI manager */
NOESISGUI_API INoesisGuiManagerPtr GetGuiManager();

/**
* Get Noesis widget factory */
NOESISGUI_API IWidgetFactoryPtr GetWidgetFactory();
