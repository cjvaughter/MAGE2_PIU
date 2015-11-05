/*******************************************************************************
* Name: Haptic.cpp
* Description: Haptic Feedback Library
* Author: Jacob Dixon & CJ Vaughter
*******************************************************************************/

#include "Haptic.h"

HapticClass Haptic;

void HapticClass::init()
{
	DDRF |= 0x03;
}

void HapticClass::pulse(uint8_t direction, uint8_t pulseCount, uint16_t pulseOnTime, uint16_t pulseOffTime) //pulse length and pulse time off are in milliseconds
{
	_pulse = 0;
	_direction = direction;
	_pulseCount = pulseCount;
	_pulseOnTime = pulseOnTime;
	_pulseOffTime = pulseOffTime;
	_nextTime = 0;
	_on = false;
}

void HapticClass::stop()
{
	_pulse = 255;
	_pulseCount = 0;
	PORTF &= ~(0x03);
}

void HapticClass::run(uint64_t time)
{
	if(_pulse < _pulseCount)
	{
		if(time >= _nextTime)
		{
			if(_on)
			{
				switch(_direction)
				{
					default:
						PORTF &= ~(0x03);
						break;
					case Left:
						PORTF &= ~(0x01);
						break;
					case Right:
						PORTF &= ~(0x02);
						break;
				}
				_nextTime = time + _pulseOffTime;
				_on = false;
				_pulse++;
			}
			else
			{
				switch(_direction)
				{
					default:
						PORTF |= 0x03;
						break;
					case Left:
						PORTF |= 0x01;
						break;
					case Right:
						PORTF |= 0x02;
						break;
				}
				_nextTime = time + _pulseOnTime;
				_on = true;
			}
		}
	}
}
