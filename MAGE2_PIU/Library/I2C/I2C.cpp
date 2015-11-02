/*******************************************************************************
* Name: I2C.cpp
* Description: I2C Library
* Author: CJ Vaughter
*******************************************************************************/

#include "I2C.h"

I2CClass I2C;

void I2CClass::begin()
{
	Wire.begin();
}

uint8_t I2CClass::writeReg(uint8_t address, uint8_t reg, uint8_t data)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(data);
	return Wire.endTransmission();
}

void I2CClass::writeRegs(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len)
{
	for (int i = 0; i < len; i++)
	{
		Wire.beginTransmission(address);
		Wire.write(reg++);
		Wire.write(buffer[i]);
		Wire.endTransmission();
	}
	
}

uint8_t I2CClass::readReg(uint8_t address, uint8_t reg)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom((int)address, (int)1);
	while(!Wire.available());
	return Wire.read();
}

void I2CClass::readRegs(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom((int)address, (int)len);
	while(Wire.available() < len);
	for(int i = 0; i < len; i++)
		buffer[i] = Wire.read(); 
}

uint8_t I2CClass::command(uint8_t address, uint8_t reg)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	return Wire.endTransmission();
}
