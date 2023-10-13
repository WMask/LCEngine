/**
* LCLocalization.h
* 29.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCDelegate.h"

#include <string>
#include <map>

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/** Localization file */
class CORE_API LcLocalization
{
public:
	typedef std::map<std::string, std::wstring> TLocMap;
	//
	LcLocalization& operator=(const LcLocalization&) = default;
	//
	LcLocalization(const LcLocalization&) = default;


public:
	LcLocalization() {}
	//
	~LcLocalization() {}
	//
	void Load(const char* filePath);
	//
	void Set(const char* key, const wchar_t* value) { entries[key] = value; }
	//
	std::wstring Get(const char* key) const;
	//
	const std::string& GetCulture() const { return culture; }


protected:
	std::string culture;
	//
	TLocMap entries;

};


/** Localization interface */
class ILocalizationManager
{
public:
	/**
	* Subscribe to get changes of culture. */
	LcDelegate<std::string, const struct LcAppContext&> onCultureChanged;


public:
	virtual ~ILocalizationManager() {}
	//
	virtual void Init(const struct LcAppContext* context) = 0;
	// add new culture
	virtual void Add(const char* filePath) = 0;
	// culture like "en", "es", "fr"
	virtual bool SetCulture(const char* culture) = 0;
	// set value
	virtual void Set(const char* key, const wchar_t* value) = 0;
	// get localized text entry
	virtual std::wstring Get(const char* key) const = 0;
	// get current culture's name
	virtual const std::string& GetCulture() const = 0;
};


/** Localization manager */
class CORE_API LcLocalizationManager : public ILocalizationManager
{
public:
	LcLocalizationManager() : context(nullptr) {}
	//
	virtual ~LcLocalizationManager() {}


public:
	//
	virtual void Init(const struct LcAppContext* inContext) { context = inContext; }
	//
	virtual void Add(const char* filePath) override;
	//
	virtual bool SetCulture(const char* culture) override;
	//
	virtual void Set(const char* key, const wchar_t* value) override;
	//
	virtual std::wstring Get(const char* key) const override;
	//
	virtual const std::string& GetCulture() const override { return culture; }


protected:
	std::map<std::string, std::shared_ptr<LcLocalization>> cultures;
	//
	std::string culture;
	//
	const struct LcAppContext* context;

};

typedef std::shared_ptr<class ILocalizationManager> TLocalizationPtr;
