/*******************************************************************************
* Name: Bluetooth.cpp
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#include "Bluetooth.h"
#include "elapsedMillis.h"

#define DELAY_MS 500 //delay between sending config commands

BluetoothClass Bluetooth;

elapsedMillis tElapsed;
unsigned int tInterval = 10000; //Bluetooth search time

void BluetoothClass::classicMasterInit()
{
	Serial2.begin(9600);
	Serial2.println("$$$");
	delay(DELAY_MS);
	Serial2.println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(115200); //factory default BAUD rate
	Serial2.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial2.println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	Serial2.println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	Serial2.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial2.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(9600);
}
void BluetoothClass::classicSlaveInit()
{
	Serial.println("Begin of Classic Slave");
	Serial2.begin(9600);
	Serial2.println("$$$");
	delay(DELAY_MS);
	Serial2.println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(115200); //factory default BAUD rate
	Serial2.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial2.println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	Serial2.println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	Serial2.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial2.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(9600);
	Serial.println("End of classic slave");
}
void BluetoothClass::classicSlaveInitSecurityMode1() //hacked together for prototyping. Need to clean up and always run @ 115200 BAUD
{
	Serial.println("Begin of Classic Slave with passkey");
	//Serial2.begin(9600);
	Serial2.begin(115200); //factory default BAUD rate
	Serial2.println("$$$");
	delay(DELAY_MS);
	Serial2.println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	//Serial2.end();
	
	Serial2.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial2.println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	Serial2.println("SA,4"); //sets to pin code mode
	delay(DELAY_MS);
	Serial2.println("SP,1234"); //sets to pin code mode
	delay(DELAY_MS);
	Serial2.println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	Serial2.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial2.println("SU,38"); //set BAUD rate to 9600
	delay(DELAY_MS);
	//Serial2.end();
	//Serial2.begin(38400);
	Serial.println("End of classic slave");
}
void BluetoothClass::BLEMasterInit()
{
	Serial2.begin(9600);
	Serial2.println("$$$");
	delay(DELAY_MS);
	Serial2.println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(115200); //factory default BAUD rate
	Serial2.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial2.println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	Serial2.println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	Serial2.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial2.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(9600);
}
void BluetoothClass::BLESlaveInit()
{
	Serial2.begin(9600);
	Serial2.println("$$$");
	delay(DELAY_MS);
	Serial2.println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(115200); //factory default BAUD rate
	Serial2.println("$$$"); //gets into config mode
	delay(DELAY_MS);
	Serial2.println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	Serial2.println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	Serial2.println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	Serial2.println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	Serial2.end();
	Serial2.begin(9600);
}

String BluetoothClass::scanClassic(String MACaddr)
{
	Serial2.println("I,10,0"); //per data sheet, this sets up a scan for 10 seconds with no device filtering
	delay(DELAY_MS);
	while (tElapsed < tInterval) //this could probably be smarter
	{
		MACaddr += Serial2.read();
	}
	return MACaddr;
}
String BluetoothClass::scanBLE(String MACaddr)
{
	Serial2.println("IL,A,0320,0190"); //per data sheet, this sets up a scan for 20 seconds with no device filtering, sets scan rates
	delay(DELAY_MS);
	while (tElapsed < tInterval) //this could probably be smarter, MAC address needs to be parsed out still
	{
		MACaddr += Serial2.read();
	}
	return MACaddr;
}
void BluetoothClass::connectClassic(String MACaddr)
{
	Serial2.println("C," + MACaddr);
	delay(DELAY_MS);
}
void BluetoothClass::connectBLE(String MACaddr)
{
	Serial2.println("C,0," + MACaddr);
	delay(DELAY_MS);
}
void BluetoothClass::disconnect()
{
	Serial2.println("K,1");
	delay(DELAY_MS);
}

void BluetoothClass::setName(String name)
{
	Serial2.println("SN," + name);
	delay(DELAY_MS);
}

void BluetoothClass::reboot()
{
	Serial2.println("R,1");
	delay(DELAY_MS);
}
void BluetoothClass::reset()
{
	Serial2.println("SF,1");
	delay(DELAY_MS);
}

void BluetoothClass::displayFirmware()
{
	Serial2.println("V");
	delay(DELAY_MS);
	while (Serial2.available())
	{
		Serial.print(Serial2.read());//<- change this to the correct serial port later
	}
}
void BluetoothClass::displayConfiguration()
{
	Serial2.println("X");
	delay(DELAY_MS);
	while (Serial2.available())
	{
		Serial.print(Serial2.read());//<- change this to the correct serial port later
	}
}
