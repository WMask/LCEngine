/**
* GUIManager.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUIModule.h"
#include "RenderSystem/RenderSystem.h"
#include "Core/LCTypes.h"


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
	void Init(bool useNoesis);
	/**
	* Shutdown GUI manager */
	void Shutdown();


protected:
	LCGUIManager();
	//
	LCGUIManager(const LCGUIManager&);
	//
	LCGUIManager& operator=(const LCGUIManager&);


protected:
	bool useNoesis;

};
