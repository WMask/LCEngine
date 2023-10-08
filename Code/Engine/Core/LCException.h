/**
* LCException.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#include <exception>
#include <string>


/**
* Exception class
*/
class LcException : public std::exception
{
public:
	LcException() = default;
	//
	LcException(const LcException&) = default;
	//
	LcException& operator=(const LcException&) = default;
	//
	LcException(const std::string& inMessage) : message(inMessage) {}
	//
	LcException(const std::exception& prevEx, const char* curLocation)
	{
		message = prevEx.what();
		message += ">\n";
		message += curLocation;
	}
	//
	LcException(const std::exception& prevEx, const char* preMsg, const char* msg, const char* postMsg)
	{
		message = prevEx.what();
		message += ">\n";
		message += preMsg;
		message += msg;
		message += postMsg;
	}


public:// std::exception interface implementation
	virtual char const* what() const override { return message.c_str(); }


protected:
	std::string message;

};


// Start catch block
#define LC_CATCH } catch (const std::exception& ex)
// Throw appended exception
#define LC_THROW(curLocation) throw LcException(ex, curLocation);
// Throw appended exception
#define LC_THROW_EX(preMsg, msg, postMsg) throw LcException(ex, preMsg, msg, postMsg);
// Start try-catch block
#define LC_TRY try\
{
