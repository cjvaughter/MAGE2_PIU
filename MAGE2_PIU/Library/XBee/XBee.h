/*******************************************************************************
* Name: XBee.h
* Description: DigiMesh API Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/
 
#ifndef XBEE_h
#define XBEE_h

#include <Arduino.h>
#include <Debugger.h>

#define XB Serial1
#define BFR_SIZE 32

//XBEE API frame definitions
#define Delimiter 0x7E
#define TX 0x10
#define TX_Header 0x0E
#define RX 0x90
#define RX_Header 0x0C
#define Check 0xFF
#define DefaultAddress16High 0xFF
#define DefaultAddress16Low 0xFE

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
	ChangeWeapon,
	
	DFU = 0xFF
};

class XBeeClass : Debugger
{
	public:
		XBeeClass() : Debugger("XBEE") {}
		void init(uint64_t address);
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
		uint8_t tx_data[BFR_SIZE];
		boolean connected;
			
	private:
		uint64_t coordinator;
		boolean _msgReady;
		boolean _busy;
		uint8_t rx_data[BFR_SIZE];
		uint8_t rx_length;
		uint8_t tx_bfr[TX_Header + BFR_SIZE];
		uint8_t SendChecksum;
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
