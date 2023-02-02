/**
* NoesisGUIManager.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

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
	virtual void SetXamlProvider(Noesis::Ptr<Noesis::XamlProvider> provider) { xamlProvider = provider; }


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
	virtual void OnKeyboard(LcMouseBtn btn, int x, int y) override;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y) override;


protected:
	Noesis::Ptr<Noesis::XamlProvider> xamlProvider;
	//
	Noesis::Ptr<Noesis::IView> view;
	//
	LcSize viewportSize;

};
