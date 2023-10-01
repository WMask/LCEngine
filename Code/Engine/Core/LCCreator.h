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
template<class T, class Container>
class LcLifetimeStrategy
{
public:
	LcLifetimeStrategy() : curTypeId(-1) {}
	//
	virtual ~LcLifetimeStrategy() {}
	//
	virtual std::shared_ptr<T> Create(const void* userData) { return std::shared_ptr<T>(); }
	//
	virtual void Destroy(T& item, Container& items) {}
	// need static int GetStaticId() from each type
	int curTypeId;
};


/** Object creator */
template<
	class I,
	class Strategy = LcLifetimeStrategy<I, std::deque<std::shared_ptr<I>>>,
	class Container = std::deque<std::shared_ptr<I>>>
class LcCreator
{
public:
	typedef std::shared_ptr<Strategy> TStrategyPtr;
	//
	typedef std::shared_ptr<I> TItemPtr;
	//
	typedef Container TItemsList;


public:
	LcCreator() : userData(nullptr) {}
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
	void SetUserData(void* data)
	{
		userData = data;
	}
	//
	template<class T>
	T* Add()
	{
		strategy->curTypeId = T::GetStaticId();
		TItemPtr newItem = strategy->Create(userData);
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

			strategy->Destroy(*item, items);
			items.erase(it);
		}
	}
	//
	void Clear()
	{
		for (const TItemPtr& item : items)
		{
			strategy->Destroy(*item.get(), items);
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
	//
	void* userData;

};
