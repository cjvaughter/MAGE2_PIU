/*******************************************************************************
* Name: Debugger.cpp
* Description: Debugging library
* Author: CJ Vaughter
*******************************************************************************/

#include "Debugger.h"

Debugger::Debugger()
{
	processName = "SYSTEM";
	inBfr = new char[192];
	outBfr = new char[256];
}

Debugger::Debugger(const char* name)
{
	#ifdef DEBUG
	processName = name;
	inBfr = new char[192];
	outBfr = new char[256];
	#endif
}

void Debugger::log(const char* data)
{
	log(processName, data);
}

void Debugger::log(const char* name, const char* data)
{
	sprintf(outBfr, "[%s]-%07lu: %s", name, millis(), data);
	Serial.println(outBfr);
}