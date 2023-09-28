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
	LcLifetimeStrategy() : curTypeId(-1) {}
	//
	virtual ~LcLifetimeStrategy() {}
	//
	virtual std::shared_ptr<T> Create() { return std::shared_ptr<T>(); }
	//
	virtual void Destroy(T& item) {}
	// need static int GetStaticId() from each type
	int curTypeId;
};


/** Object creator */
template<class I, class Strategy = LcLifetimeStrategy<I>, class Container = std::deque<std::shared_ptr<I>>>
class LcCreator
{
public:
	typedef std::shared_ptr<Strategy> TStrategyPtr;
	//
	typedef Container TItemsList;


public:
	LcCreator() {}
	//
	~LcCreator()
	{
		Clear();
	}
	//
	void SetLifetimeStrategy(TStrategyPtr inStrategy)
	{
		if (inStrategy) strategy = inStrategy;
	}
	//
	template<class T>
	T* Add()
	{
		strategy->curTypeId = T::GetStaticId();
		auto newItem = strategy->Create();
		items.insert(items.end(), newItem);
		return static_cast<T*>(newItem.get());
	}
	//
	void Remove(I* item)
	{
		if (item)
		{
			auto it = std::find_if(items.begin(), items.end(), [item](const std::shared_ptr<I>& curItem) {
				return curItem.get() == item;
			});

			strategy->Destroy(*item);
			items.erase(it);
		}
	}
	//
	void Clear()
	{
		for (auto& item : items)
		{
			strategy->Destroy(*item.get());
		}

		items.clear();
	}
	//
	const TItemsList& GetItems() const { return items; }
	//
	TItemsList& GetItems() { return items; }


protected:
	TItemsList items;
	//
	TStrategyPtr strategy;

};
