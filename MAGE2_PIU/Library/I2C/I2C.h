/*******************************************************************************
* Name: I2C.h
* Description: I2C Library
* Author: CJ Vaughter
*******************************************************************************/

#ifndef I2C_h
#define I2C_h

#include <Arduino.h>
#include <Wire.h>

class I2CClass
{
	public:
		void begin();
		void writeReg(byte address, byte reg, byte data);
		void writeRegs(byte address, byte reg, byte *buffer, byte len);
		byte readReg(byte address, byte reg);
		void readRegs(byte address, byte reg, byte *buffer, byte len);
		void command(byte address, byte reg);
};

extern I2CClass I2C;

#endif
