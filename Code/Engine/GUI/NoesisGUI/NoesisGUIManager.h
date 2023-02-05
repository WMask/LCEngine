/**
* NoesisGuiManager.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <NsGui/XamlProvider.h>

#pragma warning(disable : 4251)

class LcRenderContext;


/**
* Noesis GUI manager */
class LcNoesisGuiManager : public INoesisGuiManager
{
public:
	/**
	* Default constructor */
	LcNoesisGuiManager(LcRenderContext* context);


public: // INoesisGuiManager interface implementation
	/**
	* Set XAML provider */
	virtual void NoesisInit(Noesis::Ptr<Noesis::XamlProvider> provider, const char* resources, const char* shadersPath) override;


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
	LcRenderContext* context;
	//
	LcSize viewportSize;
	//
	std::string shadersPath;
	//
	bool isInit;

};
