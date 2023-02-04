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

#pragma warning(disable : 4251)


/**
* Noesis GUI manager */
class LcNoesisGuiManager : public INoesisGuiManager
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
	virtual void Init(void* window, LcSize viewportSize) override;
	/**
	* Render GUI */
	virtual void Render();
	/**
	* Shutdown GUI manager */
	virtual void Shutdown() override;


protected:
	Noesis::Ptr<Noesis::XamlProvider> xamlProvider;
	//
	std::shared_ptr<class LcNoesisRenderContextD3D10> context;
	//
	LcSize viewportSize;
	//
	bool isInit;
	//
	friend NOESISGUI_API IWidgetFactoryPtr GetWidgetFactory(INoesisGuiManager* gui);

};
