/*******************************************************************************
* Name: Debugger.h
* Description: Debugging library
* Author: CJ Vaughter
*******************************************************************************/

#ifndef DEBUGGER_h
#define DEBUGGER_h

#include <Arduino.h>

#define MAINLOG(...) log(__VA_ARGS__)
#define MAINLOG_F(...) sprintf(inBfr, __VA_ARGS__); log(inBfr)

#ifdef DEBUG
	#define LOG(...) log(__VA_ARGS__)
	#define LOG_F(...) sprintf(inBfr, __VA_ARGS__); log(inBfr)
	#define LOG_F_AS(name, ...) sprintf(inBfr, __VA_ARGS__); log(name, inBfr)
#else
	#define LOG(...)
	#define LOG_F(...)
	#define LOG_F_AS(...)
#endif

class Debugger
{
	public:
	Debugger();
	Debugger(const char* name);

	protected:
	const char* processName;
	char* inBfr;
	char* outBfr;
	void log(const char* data);
	void log(const char* name, const char* data);
};

#endif
