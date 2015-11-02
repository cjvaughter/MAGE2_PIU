/*******************************************************************************
* Name: RGB.cpp
* Description: LTC3220 RGB Charge Pump Library
* Author: Jacob Dixon
*******************************************************************************/

#include "RGB.h"

RGBClass RGB;

void RGBClass::init()
{
	_effectTime = 0;
	_effectActive = false;
	_blink = BlinkFast;
	_grad = GradOff;
	_address = LTC3220_ADDR;
	I2C.begin();
	if(I2C.writeReg(_address, COMMAND_REG, 0) != 0)
	{
		_address = LTC3220_1_ADDR;
		I2C.writeReg(_address, COMMAND_REG, 0);
	}
	I2C.writeReg(_address, GRAD_BLINK_REG, 0);
	
	setLed(All, NoColor);
	setLed(Power, Red);
}

void RGBClass::setLed(uint8_t led, uint8_t color, uint8_t led_state, uint8_t brightness)
{
	uint8_t rgb[3];
	
	switch(color)
	{
		case Red:
			rgb[0] = brightness;
			rgb[1] = B_0;
			rgb[2] = B_0;
			break;
		case Orange:
			rgb[0] = brightness;
			rgb[1] = brightness/12;
			if(rgb[1] == 0)
			{
				rgb[0] = 12;
				rgb[1] = 1;
			}
			rgb[2] = B_0;
			break;
		case Yellow:
			rgb[0] = brightness;
			rgb[1] = brightness/6;
			if(rgb[1] == 0)
			{
				rgb[0] = 6;
				rgb[1] = 1;
			}
			rgb[2] = B_0;
			break;
		case Green:
			rgb[0] = B_0;
			rgb[1] = brightness/6;
			if(rgb[1] == 0)
			{
				rgb[1] = 1;
			}
			rgb[2] = B_0;
			break;
		case Blue:
			rgb[0] = B_0;
			rgb[1] = B_0;
			rgb[2] = brightness/6;
			if(rgb[2] == 0)
			{
				rgb[2] = 1;
			}
			break;
		case Purple:
			rgb[0] = brightness;
			rgb[1] = B_0;
			rgb[2] = brightness/6;
			if(rgb[2] == 0)
			{
				rgb[0] = 6;
				rgb[2] = 1;
			}
			break;
		case NoColor:
			rgb[0] = B_0;
			rgb[1] = B_0;
			rgb[2] = B_0;
			break;
	}
	
	rgb[0] |= led_state;
	rgb[1] |= led_state;
	rgb[2] |= led_state;
	
	switch(led)
	{
		case Power:
			if(color == NoColor) rgb[0] = B_0;
			else rgb[0] = B_100;
			I2C.writeReg(_address, Power, rgb[0]);
			break;
		case HealthBar:
			I2C.writeRegs(_address, Health1, rgb, 3);
			I2C.writeRegs(_address, Health2, rgb, 3);
			I2C.writeRegs(_address, Health3, rgb, 3);
			I2C.writeRegs(_address, Health4, rgb, 3);
			break;
		case All:
			I2C.writeRegs(_address, Health1, rgb, 3);
			I2C.writeRegs(_address, Health2, rgb, 3);
			I2C.writeRegs(_address, Health3, rgb, 3);
			I2C.writeRegs(_address, Health4, rgb, 3);
			I2C.writeRegs(_address, Team, rgb, 3);
			I2C.writeReg(_address, Power, B_100);
			break;
		default:
			I2C.writeRegs(_address, led, rgb, 3);
			break;
	}
}

void RGBClass::setBlinkRate(uint8_t rate)
{
	_blink = rate;
	I2C.writeReg(_address, GRAD_BLINK_REG, _blink | _grad);
}

void RGBClass::setGradRate(uint8_t rate)
{
	_grad = rate;
	I2C.writeReg(_address, GRAD_BLINK_REG, _blink | _grad);
}

void RGBClass::setHealth(uint8_t percent)
{
	uint8_t diff;
	_percent = percent;
	
	if(percent == 100)
	{
		setLed(HealthBar, Green, Normal, B_50);
	}
	else if(percent == 255)
	{
		setLed(HealthBar, NoColor, Normal, B_0);
	}
	else if(percent >= 80)
	{
		diff = 100 - percent;
		diff = 21 - diff;
		setLed(Health4, Green, Normal, (uint8_t)((float)diff * 1.6));
		setLed(Health3, Green, Normal, B_50);
		setLed(Health2, Green, Normal, B_50);
		setLed(Health1, Green, Normal, B_50);
	}
	else if(percent >= 75)
	{
		setLed(Health4, Green, Blink, 1);
		setLed(Health3, Green, Normal, B_50);
		setLed(Health2, Green, Normal, B_50);
		setLed(Health1, Green, Normal, B_50);
	}
	else if(percent >= 55)
	{
		diff = 75 - percent;
		diff = 21 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, Green, Normal, (uint8_t)((float)diff * 1.6));
		setLed(Health2, Green, Normal, B_50);
		setLed(Health1, Green, Normal, B_50);
	}
	else if(percent >= 50)
	{
		setLed(Health4, NoColor);
		setLed(Health3, Green, Blink, 1);
		setLed(Health2, Green, Normal, B_50);
		setLed(Health1, Green, Normal, B_50);
	}
	else if(percent >= 30)
	{
		diff = 50 - percent;
		diff = 21 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, Yellow, Normal, (uint8_t)((float)diff * 1.6));
		setLed(Health1, Yellow, Normal, B_50);
	}
	else if(percent >= 25)
	{
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, Yellow, Blink, 1);
		setLed(Health1, Yellow, Normal, B_50);
	}
	else if(percent >= 5)
	{
		diff = 25 - percent;
		diff = 21 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, NoColor);
		setLed(Health1, Red, Normal, (uint8_t)((float)diff * 1.6));
	}
	else if(percent > 0)
	{
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, NoColor);
		setLed(Health1, Red, Blink, 1);
	}
	else
	{
		setLed(HealthBar, Red, Normal, B_50);
	}
}

void RGBClass::doEffect(uint8_t effect, uint16_t length, uint64_t time)
{
	//do blink, fade, whatever here
	
	_effectTime = time + length;
	_effectActive = true;
}

void RGBClass::run(uint64_t time)
{
	if(_effectActive)
	{
		if(time >= _effectTime)
		{
			setHealth(_percent);
			_effectActive = false;
		}
	}
}