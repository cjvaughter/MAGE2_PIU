/*******************************************************************************
* Name: MIRP2.h
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#ifndef MIRP2_h
#define MIRP2_h

#include <Arduino.h>
#include <Constants.h>
#include <Debugger.h>

#define PACKET_SIZE 6
#define F_CARRIER 38000L

const uint16_t SamplePeriod = (uint16_t)(F_CPU / F_CARRIER) + 1;

class MIRP2Class : Debugger
{
	public:
		MIRP2Class() : Debugger("MIRP") {}
		void init();
		void setTimer();
		void decode();
		volatile uint8_t data[PACKET_SIZE];
		volatile boolean msgReady;
		volatile uint8_t direction;

	private:
		void reset();
		void setPinChange();
		void validate();
		uint8_t step;
		uint8_t byteCount;
		uint8_t startNibble;
		uint8_t bitPos;
		int8_t sampleCount;
		uint8_t sample;
		int8_t sampleBias;
		int8_t frontDrift;
		int8_t backDrift;
		uint8_t zeroBiasCounter;
		boolean badStart;
		boolean badPacket;
		#ifdef DEBUG
		void printWave();
		uint16_t badEncoding[PACKET_SIZE*8];
		uint8_t badEncodingCount;
		#endif
};

extern MIRP2Class MIRP2;

#endif
