/**
* NoesisGuiManager.h
* 02.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <NsGui/XamlProvider.h>
#include <NsGui/TextureProvider.h>
#include <NsGui/FontProvider.h>

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
	virtual void NoesisInit(
		Noesis::Ptr<Noesis::XamlProvider> xamls,
		Noesis::Ptr<Noesis::TextureProvider> textures,
		Noesis::Ptr<Noesis::FontProvider> fonts,
		const char* resources, const char* shadersPath) override;


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
	Noesis::Ptr<Noesis::TextureProvider> textureProvider;
	//
	Noesis::Ptr<Noesis::FontProvider> fontProvider;
	//
	LcRenderContext* context;
	//
	LcSize viewportSize;
	//
	std::string shadersPath;
	//
	bool isInit;

};
