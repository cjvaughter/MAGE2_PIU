/*******************************************************************************
* Name: Bluetooth.cpp
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#include "Bluetooth.h"

BluetoothClass Bluetooth;

boolean BluetoothClass::init()
{	
	LOG("Initializing...");

	connected = false;
	nextHeartbeat = 0;
	device_id = 0xFFFF;
	uint8_t error = 0;	
	
	DDRD |= 0x80;
	PORTD &= ~(0x80);
	
	DDRF |= 0x08;
	PORTF |= 0x08;

	delay(1000); //for module start up
	
	if(!find_baud())
	{
		LOG("Could not detect baud rate! Defaulting to 38400...");
		BT.begin(38400);
	}
	
	error += command("UART", "38400");
	error += command("RMAAD", "0");
	error += command("ROLE", "0");
	error += command("NAME", "MAGE2_PIU");
	error += command("SENM", "1,0");
	error += command("PSWD", "1234");
	error += command("CMODE", "1");
	error += command("RESET");

	if(error == 0)
	{
		LOG("Success!");
		PORTF &= ~(0x08);
		return true;
	}

	//compatibility fix for modules with EN instead of KEY //
	else if(error == 1)									   //
	{													   //
		LOG("Success!");								   //
		hw_reset();										   //
		return true;									   //
	}													   //
														   //
	hw_reset();											   //
	BT.begin(38400);									   //
	// /////////////////////////////////////////////////// //

	return false;
}

void BluetoothClass::hw_reset()
{
	PORTF &= ~(0x08);
	delay(100);
	PORTF |= 0x08;
	delay(100);
}

boolean BluetoothClass::find_baud()
{
	LOG("Detecting baud rate...");
	uint8_t error = 0;

	//try 38400 first
	BT.begin(38400);
	error = command("AT");
	if(error == 0) return true;

	for(uint32_t baud = 1200; baud < 115200; baud *= 2)
	{
		if(baud == 38400) continue;
		BT.begin(baud);
		error = command("AT");
		if(error == 0) return true;
	}
	return false;
}

boolean BluetoothClass::command(const char* cmd, const char* data)
{
	BT.print("AT+");
	BT.print(cmd);
	BT.print("=");
	BT.print(data);
	BT.print("\r\n");
	BT.flush();
	return wait_for_lf();
}

boolean BluetoothClass::command(const char* cmd)
{
	BT.print("AT+");
	BT.print(cmd);
	BT.print("\r\n");
	BT.flush();
	return wait_for_lf();
}

boolean BluetoothClass::wait_for_lf()
{
	if(BT.read() == 0x0A)
		return false; //sweet
	
	uint8_t count = 100;
	while(count--)
	{
		delay(1);
		if(BT.read() == 0x0A)
			return false; //not bad
	}
	return true; //you tried
}

void BluetoothClass::confirmConnection(uint16_t id)
{
	LOG_F("Connection confirmed - PIU ID: 0x%04X", id);
	uint8_t idHigh = (byte)(id >> 8), idLow = (byte)id;
	BT.write(BTDelimiter);
	BT.write(BTConnect);
	BT.write(idHigh);
	BT.write(idLow);
	BT.write(0x00);
	BT.write((byte)(Check - (BTConnect + idLow + idHigh)));
}

void BluetoothClass::run()
{
	uint8_t nbytes = (uint8_t)BT.available();
	while(nbytes--) read(BT.read());
}

void BluetoothClass::read(uint8_t data)
{	
	switch(_step)
	{
		case 0:
		if (data == BTDelimiter)
		{
			msgReady = false;
			_step++;
		}
		break;
		case 1:
		rx_func = data;
		if(!connected && rx_func != BTConnect)
			_step = 0;
		_sum = data;
		_step++;
		break;
		case 2:
		rx_data[0] = data;
		_sum += data;
		_step++;
		break;
		case 3:
		rx_data[1] = data;
		_sum += data;
		_step++;
		break;
		case 4:
		rx_data[2] = data;
		_sum += data;
		_step++;
		break;
		case 5:
		_checksum = data;
		if (Check - _sum == _checksum)
		{
			msgReady = true;
		}
		else
		{
			LOG_F("BAD PACKET CHECKSUM: 0x%02X - Expected 0x%02X", _checksum, Check - _sum);
		}
		_step = 0;
		break;
	}
}

void BluetoothClass::update(uint8_t health, uint8_t status, uint8_t effect)
{
	LOG_F("Sending update - HEALTH: 0x%02X, STATUS: 0x%02X, EFFECT: 0x%02X", health, status, effect);
	uint8_t sum = BTUpdate + health + status + effect;
	
	BT.write(BTDelimiter);
	BT.write(BTUpdate);
	BT.write(health);
	BT.write(status);
	BT.write(effect);
	BT.write(Check - sum);
}

void BluetoothClass::ack()
{
	BT.write(BTDelimiter);
	BT.write(BTACK);
	BT.write(0x00);
	BT.write(0x00);
	BT.write(0x00);
	BT.write(0x00);
}
