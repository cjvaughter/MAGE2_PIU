/*******************************************************************************
* Name: MIRP2.h
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#ifndef MIRP2_h
#define MIRP2_h

#include <Arduino.h>
#include <Constants.h>
#include <RGB.h>

#define PACKET_SIZE 6

const uint16_t delay_us = 395;
const uint16_t halfDelay = delay_us / 2;
const uint16_t OCR1A_2533Hz = F_CPU / 1000000L * delay_us - 1;
const uint16_t OCR1A_halfBit = F_CPU / 1000000L * halfDelay - 1;

class MIRP2Class
{
	public:
		void init();
		void setTimer(boolean half = false);
		void decode();
		void read();
		volatile uint8_t data[PACKET_SIZE];
		volatile boolean msgReady;
		volatile uint8_t direction;
		volatile uint8_t color;

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
		boolean reading;
};

extern MIRP2Class MIRP2;

#endif
