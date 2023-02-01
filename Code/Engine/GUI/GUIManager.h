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
class GUI_API LCGUIManager
{
public:
	/**
	* Returns GUI manager */
	static LCGUIManager& GetInstance();


public:
	/**
	* Destructor */
	~LCGUIManager();
	/**
	* Init GUI manager */
	void Init(LcSize viewportSize, bool useNoesis);
	/**
	* Shutdown GUI manager */
	void Shutdown();
	/**
	* Add XAML provider */
	void AddXamlProvider(Noesis::Ptr<Noesis::XamlProvider> inXamlProvider) { xamlProvider = inXamlProvider; }
	void MouseButtonDown(int x, int y);
	void MouseButtonUp(int x, int y);


protected:
	LCGUIManager();
	//
	LCGUIManager(const LCGUIManager&);
	//
	LCGUIManager& operator=(const LCGUIManager&);


protected:
	Noesis::Ptr<Noesis::XamlProvider> xamlProvider;
	//
	Noesis::Ptr<Noesis::IView> view;
	//
	LcSize viewportSize;
	//
	bool useNoesis;

};
