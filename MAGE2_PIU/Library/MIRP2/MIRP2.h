/*******************************************************************************
* Name: MIRP2.h
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#ifndef MIRP2_h
#define MIRP2_h

#include <Arduino.h>
#include <Constants.h>

const uint16_t timer_count = 62500;
const uint8_t CPU_freq = 16; //MHz
const uint16_t delay_us = 395;
const uint16_t halfDelay = delay_us / 2;
const uint16_t OCR1A_2533Hz = CPU_freq  * delay_us - 1;
const uint16_t OCR1A_halfBit = halfDelay * CPU_freq - 1;

class MIRP2Class
{
	public:
	void init_pinChange_interrupts();
	void init_timerCompare_interrupts();
	void init_arduino_led_pin();
	void init_variables();
	void disable_timer_interrupts();
	void disable_pinChange_interrupts();
	void timer_isr();
	volatile uint16_t timerCounter;
	volatile uint8_t PCINT16_value;
	volatile boolean msgReady;
	void decodedData(uint8_t pinValue); //decode the incoming transmission
	uint8_t bytes[10]; //store data until transmission is done
	uint8_t byteCount;
	uint8_t startNibble;
	uint8_t GPIO_odd;
	uint8_t GPIO_even;
	volatile boolean timerCounterDebug;
	volatile uint8_t direction;
	boolean available();
	uint8_t bytePos;
};

extern MIRP2Class MIRP2;

#endif
