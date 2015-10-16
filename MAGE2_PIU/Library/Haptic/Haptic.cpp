/*******************************************************************************
* Name: Haptic.cpp
* Description: Haptic Feedback Library
* Author: Jacob Dixon & CJ Vaughter
*******************************************************************************/

#include "Haptic.h"

HapticClass Haptic;

void HapticClass::init()
{
	DDRB |= 0x10;
}

void HapticClass::pulse(uint8_t pulseCount, uint16_t pulseOnTime, uint16_t pulseOffTime) //pulse length and pulse time off are in milliseconds
{
	_pulse = 0;
	_pulseCount = pulseCount + 1;
	_pulseOnTime = pulseOnTime;
	_pulseOffTime = pulseOffTime;
	_nextTime = 0;
	_on = false;
}

void HapticClass::run(uint64_t time)
{
	if(_pulse <= _pulseCount)
	{
		if(time >= _nextTime)
		{
			if(_on)
			{
				PORTB &= ~(0x10);
				_nextTime = time + _pulseOffTime;
				_on = false;
				_pulse++;
			}
			else
			{
				PORTB |= 0x10;
				_nextTime = time + _pulseOnTime;
				_on = true;
			}
		}
	}
}
