/*******************************************************************************
* Name: Debugger.h
* Description: Wrapper for Serial Debug Output
* Author: CJ Vaughter
*******************************************************************************/

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <Arduino.h>

enum Source
{
	MainProgram,
	XBeeLibrary,
	BluetoothLibrary,
	MIRP2Libaray,
	
	NoSrc = 0xFF
};

enum Events
{
	Initialized,
	MsgRX,
	MsgTX,
	
	NoType = 0xFF
};

class DebuggerClass
{
	public:
	uint8_t enabled;
	void init();
	void out(uint8_t src, uint8_t event_type, const char* data = NULL);
	void out(uint8_t src, const char* data = NULL);
	void out(const char* data = NULL);
};

extern DebuggerClass Debugger;

#endif
