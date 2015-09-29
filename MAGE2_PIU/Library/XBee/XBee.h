/*******************************************************************************
* Name: XBee.h
* Description: Header file for Digi's XBEE DigiMesh for use on the PIU
* Author: CJ Vaughter & Jacob Dixon
* Created: 9/21/2015 2:26:27 PM
* Updated: 9/29/2015 Rev. A - Jacob Dixon
*******************************************************************************/
 
#ifndef XBEE_h
#define XBEE_h

#include "Arduino.h"

//XBEE API frame definitions
#define Delimiter = 0x7e;
#define TX = 0x10;
#define TX_Header 
#define RX = 0x90;
#define RX_Header = 0x0C;
#define Escape = 0x7D;
#define XON = 0x11;
#define XOFF = 0x13;
#define XOR = 0x20;
#define Check = 0xFF;
#define DefaultAddress16High = 0xFF;
#define DefaultAddress16Low = 0xFE;

public struct IRID
{
    public uint8_t ID;
    public uint8_t Func;
    public uint8_t Uni;
    public long Timestamp;
};


public enum MsgFunc
{
    Heartbeat,
    Connect,
    SentSpell,
    RecievedSpell,
};
 
class XBeeClass
{
	public:
		void CurrentMessage();
		void Decode(uint8_t data);
		void Encode(uint8_t[] data);
		void MAGEMsg(long address, uint8_t[] data);
		static bool getReady;
		static bool setReady;
		long Address;
		uint8_t[] Data;
		public MsgFunc Func;
		public uint8_t ID;
		
	private:
		String _currentMessage;
		static int _step;
		static uint8_t _api;
		static uint8_t _length;
		static long _address;
		static int _address16;
		static uint8_t[] _data;
		static uint8_t _checksum;
		static bool _escape;
		static uint8_t _sum;
		static uint8_t _index;
		
};

extern XBeeClass XBee;


#endif