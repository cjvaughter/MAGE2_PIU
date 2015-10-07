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

class XBeeClass
{
	public:
		static void init(HardwareSerial* port);
		static void heartbeat();
		static void read();
		static void transparent_mode();
		static void api_mode();
		static void enter_at_mode();
		static void exit_at_mode();
		static void Decode(uint8_t data);
		static void Encode();
		static uint8_t* rx_data;
		static uint8_t* tx_bfr;
		static uint8_t* tx_data;
		static uint8_t rx_length;
		static uint8_t tx_length;
		static bool msgReady;
		
	private:
		static HardwareSerial* _port;
		static uint8_t _step;
		static uint16_t _length;
		static uint8_t _checksum;
		static bool _escape;
		static uint8_t _sum;
		static uint8_t _index;	
};

extern XBeeClass XBee;

#endif