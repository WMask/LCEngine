/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUIModule.h"
#include "RenderSystem/RenderSystem.h"
#include "Core/LCTypes.h"

#include <NsGui/XamlProvider.h>
#include <NsGui/IView.h>


/**
* GUI manager */
class GUI_API LcGUIManager
{
public:
	/**
	* Returns GUI manager */
	static LcGUIManager& GetInstance();


public:
	/**
	* Destructor */
	~LcGUIManager();
	/**
	* Init GUI manager */
	void Init(LcSize viewportSize, bool useNoesis);
	/**
	* Shutdown GUI manager */
	void Shutdown();
	/**
	* Add XAML provider */
	void AddXamlProvider(Noesis::Ptr<Noesis::XamlProvider> inXamlProvider) { xamlProvider = inXamlProvider; }
	/**
	* Add XAML provider */
	void MouseButtonDown(LcMouseBtn btn, int x, int y);
	/**
	* Add XAML provider */
	void MouseButtonUp(LcMouseBtn btn, int x, int y);


protected:
	LcGUIManager();
	//
	LcGUIManager(const LcGUIManager&);
	//
	LcGUIManager& operator=(const LcGUIManager&);


protected:
	Noesis::Ptr<Noesis::XamlProvider> xamlProvider;
	//
	Noesis::Ptr<Noesis::IView> view;
	//
	LcSize viewportSize;
	//
	bool useNoesis;

};
