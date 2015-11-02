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
		uint8_t writeReg(uint8_t address, uint8_t reg, uint8_t data);
		void writeRegs(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);
		uint8_t readReg(uint8_t address, uint8_t reg);
		void readRegs(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);
		uint8_t command(uint8_t address, uint8_t reg);
};

extern I2CClass I2C;

#endif
