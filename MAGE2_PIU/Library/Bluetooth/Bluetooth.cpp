/******************************************************************************
Bluetooth.cpp
Microchip Bluetooth Dual Mode model RB4677 Library Source File
Jacob Dixon
Original Creation Date: August 30, 2015
Version A
******************************************************************************/

#include "Bluetooth.h"

BluetoothClass Bluetooth;

void BluetoothClass::setPort(HardwareSerial* port)
{
	_port = port;
	_port->begin(9600);
}

void BluetoothClass::classicMasterInit()
{
	_port->println("$$$");
	delay(DELAY_MS);
	_port->println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	_port->begin(115200); //factory default BAUD rate
	_port->println("$$$"); //gets into config mode
	delay(DELAY_MS);
	_port->println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	_port->println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	_port->println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	_port->println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	_port->begin(9600);
}

void BluetoothClass::classicSlaveInit()
{
	_port->println("$$$");
	delay(DELAY_MS);
	_port->println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	_port->begin(115200); //factory default BAUD rate
	_port->println("$$$"); //gets into config mode
	delay(DELAY_MS);
	_port->println("SG,2"); //sets to BT classic mode
	delay(DELAY_MS);
	_port->println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	_port->println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	_port->println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	_port->begin(9600);
}

void BluetoothClass::BLEMasterInit()
{
	_port->println("$$$");
	delay(DELAY_MS);
	_port->println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	_port->begin(115200); //factory default BAUD rate
	_port->println("$$$"); //gets into config mode
	delay(DELAY_MS);
	_port->println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	_port->println("SM,1"); //sets to master mode
	delay(DELAY_MS);
	_port->println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	_port->println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	_port->begin(9600);
}

void BluetoothClass::BLESlaveInit()
{
	_port->println("$$$");
	delay(DELAY_MS);
	_port->println("SF,1"); //sets to factory defaults
	delay(DELAY_MS);
	_port->begin(115200); //factory default BAUD rate
	_port->println("$$$"); //gets into config mode
	delay(DELAY_MS);
	_port->println("SG,1"); //sets to BLE
	delay(DELAY_MS);
	_port->println("SM,0"); //sets to slave mode
	delay(DELAY_MS);
	_port->println("SY,4"); //sets TX power to max
	delay(DELAY_MS);
	_port->println("SU,96"); //set BAUD rate to 9600
	delay(DELAY_MS);
	_port->begin(9600);
}

String BluetoothClass::scanClassic(String MACaddr)
{
	_port->println("I,10,0"); //per data sheet, this sets up a scan for 10 seconds with no device filtering
	delay(DELAY_MS);
	while(timeElapsed < timeInterval) //this could probably be smarter
	{
		MACaddr += _port->read();
	}
	return MACaddr;
}

String BluetoothClass::scanBLE(String MACaddr)
{
	_port->println("IL,A,0320,0190"); //per data sheet, this sets up a scan for 20 seconds with no device filtering, sets scan rates
	delay(DELAY_MS);
	while(timeElapsed < timeInterval) //this could probably be smarter, MAC address needs to be parsed out still
	{
		MACaddr += _port->read();
	}
	return MACaddr;
}

void BluetoothClass::connectClassic(String MACaddr)
{
	_port->println("C," + MACaddr);
	delay(DELAY_MS);
}

void BluetoothClass::connectBLE(String MACaddr)
{
	_port->println("C,0," + MACaddr);
	delay(DELAY_MS);
}

void BluetoothClass::disconnect()
{
	_port->println("K,1");
	delay(DELAY_MS);
}

void BluetoothClass::setName(String name)
{
	_port->println("SN," + name);
	delay(DELAY_MS);
}

void BluetoothClass::reboot()
{
	_port->println("R,1");
	delay(DELAY_MS);
}
void BluetoothClass::reset()
{
	_port->println("SF,1");
	delay(DELAY_MS);
}

void BluetoothClass::displayFirmware()
{
	_port->println("V");
	delay(DELAY_MS);
	while(_port->available())
	{
		//SerialDebug.print(_port->read());//<- change this to the correct serial port later
		_port->read(); //temporary replacement for the above line
	}
}

void BluetoothClass::displayConfiguration()
{
	_port->println("X");
	delay(DELAY_MS);
	while(_port->available())
	{
		//SerialDebug.print(_port->read());//<- change this to the correct serial port later
		_port->read(); //temporary replacement for the above line
	}
}
