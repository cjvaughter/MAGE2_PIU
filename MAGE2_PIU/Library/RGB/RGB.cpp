/*******************************************************************************
* Name: RGB.cpp
* Description: LTC3220 RGB Charge Pump Library
* Author: Jacob Dixon
*******************************************************************************/

#include "RGB.h"

RGBClass RGB;

//chip addresses
static const byte LTC3220_ADDR = 0x1C;
static const byte LTC3220_1_ADDR = 0x1D;

//sub address registers (for each LED)
static const byte CommandReg = 0x00;
static const byte LED1R = 0x01;
static const byte LED1G = 0x02;
static const byte LED1B = 0x03;
static const byte LED2R = 0x04;
static const byte LED2G = 0x05;
static const byte LED2B = 0x06;
static const byte LED3R = 0x07;
static const byte LED3G = 0x08;
static const byte LED3B = 0x09;
static const byte LED4R = 0x0A;
static const byte LED4G = 0x0B;
static const byte LED4B = 0x0C;
static const byte TEAMLEDR = 0x0D;
static const byte TEAMLEDG = 0x0E;
static const byte TEAMLEDB = 0x0F;
static const byte POW_LED = 0x10;
static const byte UNUSED1 = 0x11;
static const byte UNUSED2 = 0x12;
static const byte GRAD_BLINK = 0x13;

// Command Reg
static const byte QuickWrite = 0x01; //forces all registers to data byte value
static const byte F1X = 0x01; //force into 1x current mode
static const byte F1P5X = 0x01; //force into 1.5x current mode
static const byte F2X = 0x01; //force into 2x current mode
static const byte SHUTDOWN = 0x01; //turns off all LED's

//Gradient and Blinking Reg
//static byte BlinkRate1 = 0x00; //blinks every .625 seconds for a period of 1.25 seconds
static const byte GradientRate1 = 0x02; // gradient rate of .24 seconds with a period of .313 seconds
static const byte GradientRate2 = 0x04; // gradient rate of .48 seconds with a period of .625 seconds
static const byte GradientRate3 = 0x06; // gradient rate of .96 seconds with a period of 1.25 seconds
//static byte BlinkRate1withGradiant1 = 0x01; //blinks every .625 seconds for a period of 1.25 seconds with a gradient of .24 seconds (rising and falling)
//static byte BlinkRate1withGradiant2 = 0x02; //blinks every .625 seconds for a period of 1.25 seconds with a gradient of .48 seconds (rising and falling)
//static byte BlinkRate1withGradiant3 = 0x03; //blinks every .625 seconds for a period of 1.25 seconds with a gradient of .96 seconds (rising and falling)
//there are more blink/gradient combos. I just did the first one.

//Data bytes
static const byte BlinkEnable = 0x40; //add this to data value for brightness
static const byte GradiantEnable = 0x80; //add this to data value for brightness
static const byte Brightness100Percent = 0x1F;
static const byte Brightness90Percent = 0x39;
static const byte Brightness80Percent = 0x33;
static const byte Brightness70Percent = 0x2D;
static const byte Brightness60Percent = 0x26;
static const byte Brightness50Percent = 0x20;
static const byte Brightness40Percent = 0x1A;
static const byte Brightness30Percent = 0x13;
static const byte Brightness20Percent = 0x0D;
static const byte Brightness10Percent = 0x06;





//Command byte register functions, use only when addressing the command register! (reg addr 0x00)
void RGBClass::SetPowerLEDOn()
{
	I2C.writeReg(LTC3220_ADDR,POW_LED,Brightness100Percent);
}
void RGBClass::SetPowerLEDOnPulse()
{
	//write gradient register first
	I2C.writeReg(LTC3220_ADDR,GradientRate3,GradiantEnable);
	//might need to wait some time here...
	int temp = Brightness100Percent + GradiantEnable;
	I2C.writeReg(LTC3220_ADDR,POW_LED,temp);
}
void RGBClass::SetRGBLEDColor(String Color, int Brightness, int LED) //note, LED 5 = team LED
{
	byte tempBrightness;
	byte R,G,B;
	switch(Brightness)
	{
		case 100 :
			tempBrightness = Brightness100Percent;
			break;
		case 90 :
			tempBrightness = Brightness90Percent;
			break;
		case 80 :
			tempBrightness = Brightness80Percent;
			break;
		case 70 :
			tempBrightness = Brightness70Percent;
			break;
		case 60 :
			tempBrightness = Brightness60Percent;
			break;
		case 50 :
			tempBrightness = Brightness50Percent;
			break;
		case 40 :
			tempBrightness = Brightness40Percent;
			break;
		case 30 :
			tempBrightness = Brightness30Percent;
			break;
		case 20 :
			tempBrightness = Brightness20Percent;
			break;
		case 10 :
			tempBrightness = Brightness10Percent;
			break;
		default :
			return;
	}
	switch(LED)
	{
		case 1 : //led 1
			R = LED1R;
			G = LED1G;
			B = LED1B;
			break;
		case 2 :
			R = LED2R;
			G = LED2G;
			B = LED2B;
			break;
		case 3 :
			R = LED3R;
			G = LED3G;
			B = LED3B;
			break;
		case 4 : 
			R = LED4R;
			G = LED4G;
			B = LED4B;
			break;
		case 5 :
			R = TEAMLEDR;
			G = TEAMLEDG;
			B = TEAMLEDB;
			break;
		default :
			return;
	}
	if(Color == "red")
	{
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
	}
	else if(Color == "green")
	{
		I2C.writeReg(LTC3220_ADDR,G,tempBrightness);
	}
	else if(Color == "blue")
	{
		I2C.writeReg(LTC3220_ADDR,B,tempBrightness);
	}
	else if(Color == "white")
	{
		I2C.writeReg(LTC3220_ADDR,B,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,G,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
	}
	else if(Color == "yellow")
	{
		I2C.writeReg(LTC3220_ADDR,G,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
	}
	else if(Color == "cyan")
	{
		I2C.writeReg(LTC3220_ADDR,B,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,G,tempBrightness);
	}
	else if(Color == "magenta")
	{
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,B,tempBrightness);
	}
	else if(Color == "party")
	{
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,B,tempBrightness);
	}
	else if(Color == "orange")
	{
		I2C.writeReg(LTC3220_ADDR,R,tempBrightness);
		I2C.writeReg(LTC3220_ADDR,G,Brightness60Percent); //NOTE: This will not scale correctly when red is dimmed. need to fix this
	}
	//plan to add more colors as necessary
}
