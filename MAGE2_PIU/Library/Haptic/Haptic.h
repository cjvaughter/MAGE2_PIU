/*******************************************************************************
* Name: Haptic.h
* Description: Haptic Feedback Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef HAPTIC_h
#define HAPTIC_h
#include <Arduino.h>

class HapticClass
{
	public:
		void init(uint8_t gpioPin);
		void pulseMotor(uint8_t pulseCount, uint8_t pulseLength, uint8_t pulseTimeOff);
	private:
		uint8_t gpioPin;
		uint8_t pulseCount;
		uint8_t pulseLength;
		uint8_t pulseTimeOff;
};

extern HapticClass Haptic;

#endif
