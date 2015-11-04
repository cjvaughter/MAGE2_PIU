/*******************************************************************************
* Name: Debugger.cpp
* Description: Wrapper for Serial Debug Output
* Author: CJ Vaughter
*******************************************************************************/

#include "Debugger.h"

DebuggerClass Debugger;

#ifdef DEBUGGER_ENABLED
void DebuggerClass::init()
{
	DBG.begin(115200);
}

void DebuggerClass::out(uint8_t src, uint8_t event_type, const char* data)
{
#ifndef DEBUG
	if(!enabled) return;
#endif
	
	switch(src)
	{
		case MainProgram:
			DBG.print("MAIN: ");
			break;
		case XBeeLibrary:
			DBG.print("XBEE: ");
			break;
		case BluetoothLibrary:
			DBG.print("BT: ");
			break;
		case MIRP2Libaray:
			DBG.print("IR: ");
			break;
	}
	
	switch(event_type)
	{
		case Initialized:
			DBG.print("Initialized");
			break;
		case MsgRX:
			DBG.print("RX - ");
			break;
		case MsgTX:
			DBG.print("TX - ");
			break;
	}
	
	DBG.write(data);
	DBG.println();
	DBG.flush();
}

void DebuggerClass::out(uint8_t src, const char* data)
{
	out(src, NoType, data);
}

void DebuggerClass::out(const char* data)
{
	out(NoSrc, NoType, data);
}

#else
void DebuggerClass::init() {}
void DebuggerClass::out(uint8_t src, uint8_t event_type, const char* data) {}
void DebuggerClass::out(uint8_t src, const char* data) {}
void DebuggerClass::out(const char* data) {}
#endif