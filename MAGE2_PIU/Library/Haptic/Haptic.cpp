/*******************************************************************************
* Name: Haptic.cpp
* Description: Haptic Feedback Library
* Author: Jacob Dixon
*******************************************************************************/

#include "Haptic.h"

HapticClass Haptic;

void HapticClass::init(uint8_t gpioPin)
{
	pinMode(gpioPin, OUTPUT);
}

/* //broken gpioPin call
void HapticClass::pulseMotor(uint8_t pulseCount, uint8_t pulseLength, uint8_t pulseTimeOff) //pulse length and pulse time off are in milliseconds
{
	#ifdef DEBUG
	Serial.print("\nRunning motor on pin " + gpioPin);
	Serial.println("...");
	#endif
	for(int i = 0; i < pulseCount; i++)
	{
		digitalWrite(gpioPin,HIGH);
		delay(pulseLength);
		digitalWrite(gpioPin,LOW);
		delay(pulseTimeOff);
	}
	#ifdef DEBUG
	Serial.println("Finished running motor");
	#endif
}
*/

//quick fix...
void HapticClass::pulseMotor(uint8_t pulseCount, uint8_t pulseLength, uint8_t pulseTimeOff) //pulse length and pulse time off are in milliseconds
{
	#ifdef DEBUG
	Serial.print("\nRunning motor on pin 10");
	Serial.println("...");
	#endif
	for(int i = 0; i < pulseCount; i++)
	{
		digitalWrite(10,HIGH);
		delay(pulseLength);
		digitalWrite(10,LOW);
		delay(pulseTimeOff);
	}
	#ifdef DEBUG
	Serial.println("Finished running motor");
	#endif
}
