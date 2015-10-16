/*******************************************************************************
* Name: Debugger.h
* Description: Wrapper for Serial Debug Output
* Author: CJ Vaughter
*******************************************************************************/

#ifndef DEBUGGER_h
#define DEBUGGER_h

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
	boolean enabled;
	void init();
	void out(uint8_t src, uint8_t event_type, const char* data = NULL);
	void out(uint8_t src, const char* data = NULL);
	void out(const char* data = NULL);
};

extern DebuggerClass Debugger;

#endif
