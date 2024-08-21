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
	LcLifetimeStrategy() : curTypeId(-1), layer2D(0.0f) {}
	//
	virtual ~LcLifetimeStrategy() {}
	//
	virtual std::shared_ptr<T> Create() { return std::shared_ptr<T>(); }
	//
	virtual void Destroy(T& item, Container& items) {}
	//
	float layer2D;
	// needs static int GetStaticId() from type in LcCreator::Add
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
	typedef std::unique_ptr<Strategy> TStrategyPtr;
	//
	typedef std::shared_ptr<I> TItemPtr;
	//
	typedef Container TItemsList;
	//
	typedef typename TItemsList::iterator TItemIterator;


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
		if (inStrategy) strategy = std::move(inStrategy);
	}
	//
	template<class T>
	T* Add(float layer2D = 0.0f)
	{
		strategy->curTypeId = T::GetStaticId();
		strategy->layer2D = layer2D;
		TItemPtr newItem = strategy->Create();
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
	void Clear(const TItemIterator& begin, const TItemIterator& end)
	{
		for (auto it = begin; it != end; ++it)
		{
			strategy->Destroy(*it->get(), items);
		}

		items.clear();
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

};
