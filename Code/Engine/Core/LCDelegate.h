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
template<typename D>
class LcDelegate
{
	typedef std::function<D> DType;

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
	const std::deque<DType>& GetListeners() const { return listeners; }


protected:
	std::deque<DType> listeners;

};
