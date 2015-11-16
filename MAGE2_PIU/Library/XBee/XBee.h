/*******************************************************************************
* Name: XBee.h
* Description: DigiMesh API Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/
 
#ifndef XBEE_h
#define XBEE_h

#include <Arduino.h>

#define XB Serial1

//XBEE API frame definitions
#define Delimiter 0x7e
#define TX 0x10
#define TX_Header 0x0E
#define RX 0x90
#define RX_Header 0x0C
#define Escape 0x7D
#define XON 0x11
#define XOFF 0x13
#define XOR 0x20
#define Check 0xFF
#define DefaultAddress16High 0xFF
#define DefaultAddress16Low 0xFE
#define Coordinator0 0x00
#define Coordinator1 0x7D //Escape character
#define Coordinator2 0x33 //escaped value
#define Coordinator3 0xA2
#define Coordinator4 0x00
#define Coordinator5 0x40
#define Coordinator6 0x93
#define Coordinator7 0x77
#define Coordinator8 0xC3
#define SendChecksum 0xCF

enum XBeeMsg
{
	Heartbeat,
	Connect,
	Disconnect,
	Health,
	State,
	Effect,
	Update,
    Spell_RX,
    Spell_TX,
	
	DFU = 0xFF
};

class XBeeClass
{
	public:
		void init();
		void connect(uint16_t player_id, uint16_t device_id);
		boolean available();
		void discard();
		void heartbeat();
		void run(uint64_t time);
		uint8_t nextByte();
		void transparent_mode();
		void api_mode();
		void enter_at_mode();
		void exit_at_mode();
		void wait_for_cr();
		void Decode(uint8_t data);
		void Encode(uint8_t length);
		uint8_t* tx_data;
		boolean connected;
			
	private:
		boolean _msgReady;
		boolean _busy;
		uint8_t* rx_data;
		uint8_t rx_length;
		uint8_t* tx_bfr;
		uint8_t _step;
		uint16_t _length;
		uint8_t _checksum;
		boolean _escape;
		uint8_t _sum;
		uint8_t _index;
		uint8_t _msg_index;	
		uint64_t currentTime;
		uint64_t nextTime;
};

extern XBeeClass XBee;

#endif
