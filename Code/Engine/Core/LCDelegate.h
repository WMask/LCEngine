/**
* LCDelegate.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <functional>
#include <deque>


/**
* Multicast delegate class
*/
template<typename ...A>
class LcDelegate
{
	typedef std::function<void(A...)> DType;

public:
	LcDelegate() = default;
	//
	LcDelegate(const LcDelegate&) = default;
	//
	LcDelegate& operator=(const LcDelegate&) = default;
	//
	void AddListener(const DType& listener) { listeners.push_back(listener); }
	//
	void RemoveListener(const DType& listener) { listeners.erase(listener); }
	//
	void Broadcast(A... a)
	{
		for (DType& listener : listeners)
		{
			listener(a...);
		}
	}


protected:
	std::deque<DType> listeners;

};
