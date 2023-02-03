/**
* NoesisGuiManager.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

// copy NoesisGUI code to LCEngine/Code/Engine/GUI/NoesisGUI
#include <NsGui/XamlProvider.h>
#include <NsGui/IView.h>

#pragma warning(disable : 4251)


/**
* Noesis GUI manager */
class NOESISGUI_API LcNoesisGuiManager : public INoesisGuiManager
{
public:
	/**
	* Default constructor */
	LcNoesisGuiManager();


public: // INoesisGuiManager interface implementation
	/**
	* Set XAML provider */
	virtual void NoesisInit(Noesis::Ptr<Noesis::XamlProvider> provider, const std::string& inResources) override;


public: // IGuiManager interface implementation
	/**
	* Destructor */
	virtual ~LcNoesisGuiManager() override;
	/**
	* Init GUI manager */
	virtual void Init(LcSize viewportSize) override;
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() override;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y) override;


protected:
	Noesis::Ptr<Noesis::XamlProvider> xamlProvider;
	//
	std::string resources;
	//
	LcSize viewportSize;
	//
	bool isInit;

};
