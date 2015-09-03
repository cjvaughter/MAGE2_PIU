/******************************************************************************
RN4677BT.h
Microchip Bluetooth Dual Mode model RB4677 Library Source File
Jacob Dixon
Original Creation Date: August 30, 2015
Version A
******************************************************************************/

#include "RN4677BT.h"
#include <elapsedMillis.h>

#DEFINE DELAY_MS 500 //delay between sending config commands

RB4677BTClass bluetooth;
elapsedMillis timeElapsed;
unsigned int timeInterval = 10000; //Bluetooth search time

void RB4677BTClass::classicMasterInit()
{
	Serial.begin(9600);
	Serial.println("$$$");
	delay(DELAY_MS);
	Serial.println("SF,1") //sets to factory defaults
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(115200); //factory default BAUD rate
	Serial.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial.println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	Serial.println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	Serial.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(9600);
}
void RB4677BTClass::classicSlaveInit()
{
	Serial.begin(9600);
	Serial.println("$$$");
	delay(DELAY_MS);
	Serial.println("SF,1") //sets to factory defaults
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(115200); //factory default BAUD rate
	Serial.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial.println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	Serial.println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	Serial.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(9600);
}
void RB4677BTClass::BLEMasterInit()
{
	Serial.begin(9600);
	Serial.println("$$$");
	delay(DELAY_MS);
	Serial.println("SF,1") //sets to factory defaults
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(115200); //factory default BAUD rate
	Serial.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial.println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	Serial.println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	Serial.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(9600);
}
void RB4677BTClass::BLESlaveInit()
{
	Serial.begin(9600);
	Serial.println("$$$");
	delay(DELAY_MS);
	Serial.println("SF,1") //sets to factory defaults
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(115200); //factory default BAUD rate
	Serial.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial.println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	Serial.println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	Serial.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial.end();
	Serial.begin(9600);
}

void RB4677BTClass::scanClassic(String MACaddr)
{
	Serial.println("I,10,0"); //per data sheet, this sets up a scan for 10 seconds with no device filtering
	delay(DELAY_MS);
	while(timeElapsed < timeInterval) //this could probably be smarter
	{
		MACaddr += Serial.read();
	}
	return MACaddr;
}
void RB4677BTClass::scanBLE(String MACaddr)
{
	Serial.println("IL,A,0320,0190"); //per data sheet, this sets up a scan for 20 seconds with no device filtering, sets scan rates
	delay(DELAY_MS);
	while(timeElapsed < timeInterval) //this could probably be smarter, MAC address needs to be parsed out still
	{
		MACaddr += Serial.read();
	}
	return MACaddr;
}
void RB4677BTClass::connectClassic(String MACaddr)
{
	Serial.println("C," + MACaddr);
	delay(DELAY_MS);
}
void RB4677BTClass::connectBLE(String MACaddr)
{
	Serial.println("C,0," + MACaddr);
	delay(DELAY_MS);
}
void RB4677BTClass::disconnect()
{
	Serial.println("K,1");
	delay(DELAY_MS);

void RB4677BTClass::setName(String name)
{
	Serial.println("SN," + name);
	delay(DELAY_MS);
}

void RB4677BTClass::reboot()
{
	Serial.println("R,1");
	delay(DELAY_MS);
}
void RB4677BTClass::reset()
{
	Serial.println("SF,1");
	delay(DELAY_MS);
}

void RB4677BTClass::displayFirmware()
	Serial.println("V");
	delay(DELAY_MS);
	while(Serial.available())
	{
		SerialDebug.print(Serial.read());//<- change this to the correct serial port later
	}
void RB4677BTClass::displayConfiguration()
{
	Serial.println("X");
	delay(DELAY_MS);
	while(Serial.available())
	{
		SerialDebug.print(Serial.read());//<- change this to the correct serial port later
	}
}