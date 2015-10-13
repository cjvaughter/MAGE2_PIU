/*******************************************************************************
* Name: RGB.h
* Description: LTC3220 RGB Charge Pump Library
* Author: Jacob Dixon
*******************************************************************************/
 
#ifndef RGB_h
#define RGB_h

#include <Arduino.h>
#include <I2C.h>

class RGBClass
{
	public:		
		//LTC 3220 Functions
		void SetPowerLEDOn();
		void SetPowerLEDOnPulse();
		void SetRGBLEDColor(String Color, int Brightness, int LED);
		void SetRGBLEDColorGradiant(String Color, int Brightness, String GradBlink, int LED);
	private:
};

extern RGBClass RGB;

#endif
