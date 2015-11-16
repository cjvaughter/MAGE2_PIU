/*******************************************************************************
* Name: MIRP2.h
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#ifndef MIRP2_h
#define MIRP2_h

#include <Arduino.h>
#include <RGB.h>
#include "Constants.h"

#define PACKET_SIZE 6
#define F_CARRIER 38000L
#define BIT_TIME 15L

const uint16_t BitPeriod = (uint16_t)((F_CPU / (F_CARRIER / BIT_TIME)) / 1);
const uint16_t HalfBitPeriod = (uint16_t)(BitPeriod / 2L);

class MIRP2Class
{
	public:
		void init();
		void setTimer(boolean half = false);
		void decode();
		volatile uint8_t data[PACKET_SIZE];
		volatile boolean msgReady;
		volatile uint8_t direction;
		volatile uint8_t color;
		uint8_t last;
		uint8_t lastColor;

	private:
		void reset();
		void setPinChange();
		void validate();
		uint8_t step;
		uint8_t byteCount;
		uint8_t startNibble;
		uint8_t GPIO_odd;
		uint8_t GPIO_even;
		uint8_t bitPos;
};

extern MIRP2Class MIRP2;

#endif
