/*******************************************************************************
* Name: Haptic.h
* Description: Haptic Feedback Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef HAPTIC_h
#define HAPTIC_h

#include <Arduino.h>
#include <Constants.h>
#include <Debugger.h>

class HapticClass : Debugger
{
	public:
		HapticClass() : Debugger("HAPTIC") {}
		void init();
		void pulse(uint8_t direction, uint8_t pulseCount, uint16_t pulseOnTime, uint16_t pulseOffTime = 0);
		void stop();
		void run(uint64_t time);
	private:
		uint8_t _pulse;
		uint8_t _direction;
		uint8_t _pulseCount;
		uint16_t _pulseOnTime;
		uint16_t _pulseOffTime;
		boolean _on;
		uint64_t _nextTime;
};

extern HapticClass Haptic;

#endif
