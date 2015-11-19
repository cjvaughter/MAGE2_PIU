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
	_effectToggle = false;
	_effectColor = NoColor;
	_health = 0;
	_directionTime = 0;
	_directionActive = false;
	_stunned = false;
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
	delay(250);
	setLed(Power, Red, B_100);
}

void RGBClass::setLed(uint8_t led, uint8_t color, uint8_t brightness, uint8_t led_state)
{
	uint8_t rgb[3];
	uint8_t calibratedBrightness = brightness/6;
	if(calibratedBrightness == 0) calibratedBrightness = 1;
	
	switch(color)
	{
		case Red:
			rgb[0] = brightness;
			rgb[1] = B_0;
			rgb[2] = B_0;
			break;
		case Orange:
			rgb[0] = brightness;
			rgb[1] = calibratedBrightness / 4;
			rgb[2] = B_0;
			if(rgb[1] == 0) rgb[1] = 1;
			break;
		case Yellow:
			rgb[0] = brightness;
			rgb[1] = calibratedBrightness;
			rgb[2] = B_0;
			break;
		case Green:
			rgb[0] = B_0;
			if(brightness == B_100)
				rgb[1] = brightness;
			else
				rgb[1] = calibratedBrightness;
			rgb[2] = B_0;
			break;
		case Cyan:
			rgb[0] = B_0;
			rgb[1] = calibratedBrightness;
			rgb[2] = calibratedBrightness;
			break;
		case Blue:
			rgb[0] = B_0;
			rgb[1] = B_0;
			if(brightness == B_100)
				rgb[2] = brightness;
			else
				rgb[2] = calibratedBrightness;
			break;
		case Purple:
			rgb[0] = brightness;
			rgb[1] = B_0;
			rgb[2] = calibratedBrightness;
			break;
		case White:
			rgb[0] = brightness;
			rgb[1] = calibratedBrightness;
			rgb[2] = calibratedBrightness;
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
	_health = percent;
}

void RGBClass::showHealth()
{
	_directionActive = false;
	_effectActive = false;
	
	uint8_t diff;
	if(_health == 100)
	{
		setLed(HealthBar, Green);
	}
	else if(_health >= 75)
	{
		diff = 100 - _health;
		diff = 26 - diff;
		setLed(Health4, Green, (uint8_t)((float)diff * 2.42));
		setLed(Health3, Green);
		setLed(Health2, Green);
		setLed(Health1, Green);
	}
	else if(_health >= 50)
	{
		diff = 75 - _health;
		diff = 26 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, Green, (uint8_t)((float)diff * 2.42));
		setLed(Health2, Green);
		setLed(Health1, Green);
	}
	else if(_health >= 25)
	{
		diff = 50 - _health;
		diff = 26 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, Green, (uint8_t)((float)diff * 2.42));
		setLed(Health1, Green);
	}
	else if(_health > 0)
	{
		diff = 25 - _health;
		diff = 26 - diff;
		setLed(Health4, NoColor);
		setLed(Health3, NoColor);
		setLed(Health2, NoColor);
		setLed(Health1, Green, (uint8_t)((float)diff * 2.42));
	}
	else
	{
		setLed(HealthBar, Red);
	}
}

void RGBClass::setDirection(uint8_t color, uint8_t direction, uint64_t time, boolean stunned)
{
	setLed(HealthBar, NoColor);
	
	switch (direction)
	{
		case Front:
			setLed(Health3, color);
			setLed(Health2, color);
			break;
		case Back:
			setLed(Health4, color);
			setLed(Health1, color);
			break;
		case Left:
			setLed(Health1, color);
			break;
		case Right:
			setLed(Health4, color);
			break;
	}
	
	_stunned = stunned;
	_direction = direction;
	_directionActive = true;
	_directionState = 0;
	_directionColor = color;
	_directionTime = time + 100;
}

void RGBClass::setEffect(uint8_t color)
{
	_effectColor = color;
}

void RGBClass::doEffect(uint64_t time)
{
	if(_effectColor == NoColor)
	{
		_effectActive = false;
		return;
	}
	_effectActive = true;
	_effectToggle = false;
	_effectTime = time;
}

void RGBClass::run(uint64_t time)
{
	if(_directionActive)
	{
		if(time >= _directionTime)
		{
			if(_direction != Front && _direction != Back)
				setLed(HealthBar, NoColor);
				
			switch(_directionState)
			{
				case 0:
					switch (_direction)
					{
						case Front: case Back:
							//do nothing
							break;
						case Left:
							setLed(Health1, _directionColor);
							setLed(Health2, _directionColor);
							break;
						case Right:
							setLed(Health4, _directionColor);
							setLed(Health3, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 1:
					switch (_direction)
					{
						case Front: case Back:
							//do nothing
							break;
						case Left:
							setLed(Health1, _directionColor);
							setLed(Health2, _directionColor);
							setLed(Health3, _directionColor);
							break;
						case Right:
							setLed(Health4, _directionColor);
							setLed(Health3, _directionColor);
							setLed(Health2, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 2:
					switch (_direction)
					{
						case Front:
							setLed(HealthBar, NoColor);
							setLed(Health4, _directionColor);
							setLed(Health1, _directionColor);
							break;
						case Back:
							setLed(HealthBar, NoColor);
							setLed(Health2, _directionColor);
							setLed(Health3, _directionColor);
							break;
						case Left: case Right:
							setLed(Health1, _directionColor);
							setLed(Health2, _directionColor);
							setLed(Health3, _directionColor);
							setLed(Health4, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 3:
					switch (_direction)
					{
						case Front: case Back:
							//do nothing
							break;
						case Left:
							setLed(Health2, _directionColor);
							setLed(Health3, _directionColor);
							setLed(Health4, _directionColor);
							break;
						case Right:
							setLed(Health3, _directionColor);
							setLed(Health2, _directionColor);
							setLed(Health1, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 4:
					switch (_direction)
					{
						case Front: case Back:
							//do nothing
							break;
						case Left:
							setLed(Health3, _directionColor);
							setLed(Health4, _directionColor);
							break;
						case Right:
							setLed(Health2, _directionColor);
							setLed(Health1, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 5:
					switch (_direction)
					{
						case Front: case Back:
							//do nothing
							break;
						case Left:
							setLed(Health4, _directionColor);
							break;
						case Right:
							setLed(Health1, _directionColor);
							break;
					}
					_directionState++;
					break;
				case 6:
					switch (_direction)
					{
						case Front: case Back:
							setLed(HealthBar, NoColor);
							break;
						case Left: case Right:
							//do nothing
							break;
					}
					_directionState++;
					break;
				case 7:
					if(_stunned)
						setLed(HealthBar, Blue);
					else
						showHealth();
					_directionActive = false;
					break;
			}
			_directionTime = time + 100;
		}
	}
	else if(_effectActive)
	{
		if(time >= _effectTime)
		{
			if(_effectToggle)
			{
				if(_stunned)
				{
					setLed(HealthBar, Blue);
				}
				else
				{
					setHealth(_health);
				}
				_effectToggle = false;
			}
			else
			{
				setLed(Health4, _effectColor);
				_effectToggle = true;
			}
			_effectTime = time + 250;
		}
	}
}