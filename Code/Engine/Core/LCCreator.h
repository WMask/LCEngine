/**
* LCCreator.h
* 19.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <memory>
#include <deque>


/** Default lifetime strategy */
template<class T>
class LcLifetimeStrategy
{
public:
	LcLifetimeStrategy() {}
	//
	virtual ~LcLifetimeStrategy() {}
	//
	virtual std::shared_ptr<T> Create() { return std::shared_ptr<T>(); }
	//
	virtual void Destroy(T& item) {}
};


/** Object creator */
template<class I, class T, class S = LcLifetimeStrategy<I>>
class LcCreator : public ICreator<I>
{
public:
	typedef std::shared_ptr<S> TStrategyType;
	typedef std::deque<std::shared_ptr<I>> TCreatorItemsList;


public:
	LcCreator() : lifetimeStrategy(std::make_shared<S>()) {}
	//
	~LcCreator()
	{
		for (auto& item : items)
		{
			lifetimeStrategy->Destroy(*item.get());
		}

		items.clear();
	}
	//
	template<class C>
	C* AddT()
	{
		items.push_back(lifetimeStrategy->Create());
		return static_cast<C*>(items.back().get());
	}
	//
	virtual void SetLifetimeStrategy(TStrategyType strategy)
	{
		if (strategy) lifetimeStrategy = strategy;
	}


public: // ICreator interface implementation
	//
	virtual I* Add() override
	{
		return AddT<I>();
	}
	//
	virtual void Remove(I* inItem) override
	{
		if (inItem)
		{
			auto it = std::find_if(items.begin(), items.end(), [inItem](const std::shared_ptr<I>& item) {
				return item.get() == inItem;
				});

			lifetimeStrategy->Destroy(*inItem);
			items.erase(it);
		}
	}
	//
	virtual const TCreatorItemsList& GetList() const override { return items; }
	//
	virtual TCreatorItemsList& GetList() override { return items; }


protected:
	TCreatorItemsList items;
	//
	TStrategyType lifetimeStrategy;

};
