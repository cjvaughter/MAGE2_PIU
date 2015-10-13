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

void I2CClass::writeReg(byte address, byte reg, byte data)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(data);
	Wire.endTransmission();
}

void I2CClass::writeRegs(byte address, byte reg, byte *buffer, byte len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	for (int i = 0; i < len; i++)
		Wire.write(buffer[i]);
	Wire.endTransmission();
}

byte I2CClass::readReg(byte address, byte reg)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom((int)address, (int)1);
	while(!Wire.available());
	return Wire.read();
}

void I2CClass::readRegs(byte address, byte reg, byte *buffer, byte len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom((int)address, (int)len);
	while(Wire.available() < len);
	for(int i = 0; i < len; i++)
		buffer[i] = Wire.read(); 
}

void I2CClass::command(byte address, byte reg)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission();
}
