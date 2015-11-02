/*******************************************************************************
* Name: RGB.h
* Description: LTC3220 RGB Charge Pump Library
* Author: Jacob Dixon
*******************************************************************************/
 
#ifndef RGB_h
#define RGB_h

#include <Arduino.h>
#include <I2C.h>

#define LTC3220_ADDR 0x1C
#define LTC3220_1_ADDR 0x1D

#define COMMAND_REG 0x00
#define GRAD_BLINK_REG 0x13

// Command Reg
#define QUICKWRITE 0x01 //forces all registers to data byte value
#define F_1X 0x06		//force into 1x current mode
#define F_1P5X 0x02		//force into 1.5x current mode
#define F_2X 0x04		//force into 2x current mode
#define SHUTDOWN 0x08	//turns off all LED's

//Gradient and Blinking Reg
#define GRAD_UP_DOWN 0x01

enum Brightness
{
	B_0   = 0x00,
	B_10  = 0x06,
	B_20  = 0x0D,
	B_30  = 0x13,
	B_40  = 0x1A,
	B_50  = 0x20,
	B_60  = 0x26,
	B_70  = 0x2D,
	B_80  = 0x33,
	B_90  = 0x39,
	B_100 = 0x3F,
};

enum Colors
{
	Red,
	Orange,
	Yellow,
	Green,
	Blue,
	Purple,
	NoColor = 0xFF,
};

enum LEDs
{
	Health1   = 0x01,
	Health2   = 0x04,
	Health3   = 0x07,
	Health4   = 0x0A,
	Team	  = 0x0D,
	Power	  = 0x10,
	HealthBar = 0xFE,
	All		  = 0xFF,
};

enum LEDState
{
	Normal = 0x00,
	Blink  = 0x40,
	Grad   = 0x80,
};

enum BlinkRate
{
	BlinkFast     = 0x00,
	BlinkFastBlip = 0x08,
	BlinkSlow     = 0x10,
	BlinkSlowBlip = 0x18,
};

enum GradRate
{
	GradOff    = 0x00,
	GradFast   = 0x02,
	GradMedium = 0x04,
	GradSlow   = 0x06,	
};

class RGBClass
{
	public:
		void init();
		void setLed(uint8_t led, uint8_t color, uint8_t led_state = Normal, uint8_t brightness = 100);
		void setBlinkRate(uint8_t rate = BlinkFast);
		void setGradRate(uint8_t rate = GradOff);
		void setHealth(uint8_t percent);
		void doEffect(uint8_t effect, uint16_t length, uint64_t time);
		void run(uint64_t time);
	private:
		uint8_t _address;
		uint8_t _blink;
		uint8_t _grad;
		uint8_t _percent;
		uint64_t _effectTime;
		boolean _effectActive;
};

extern RGBClass RGB;

#endif
