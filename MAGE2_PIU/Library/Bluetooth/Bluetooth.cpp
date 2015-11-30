/*******************************************************************************
* Name: Bluetooth.cpp
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#include "Bluetooth.h"

BluetoothClass Bluetooth;

boolean BluetoothClass::init()
{	
	connected = false;
	uint8_t error = 0;	
	
	DDRD |= 0x80;
	PORTD &= ~(0x80);
	
	DDRF |= 0x08;
	PORTF |= 0x08;
	
	delay(500); //for module to start up
	
	//if(!find_baud()) return false;
	
	//error += command("RMAAD");
	//error += command("ORGL");
	//PORTF &= ~(0x08);
	//error += command("RESET");
	//if(error) return false;
	//delay(2000);
	
	//PORTF |= 0x08;
	//BT.begin(9600);
	
	if(!find_baud()) return false;
	
	error += command("RMAAD", "0");
	error += command("ROLE", "0");
	error += command("NAME", "MAGE2_PIU");
	error += command("PSWD", "1234");
	//error += command("CLASS", "020710"); //Class is Networking Wearable (Helmet)
	PORTF &= ~(0x08);
	error += command("RESET");

	return (error == 0);
}

boolean BluetoothClass::find_baud()
{
	uint8_t error = 0;
	for(uint32_t baud = 1200; baud < 115200; baud *= 2)
	{
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

void BluetoothClass::run()
{
	uint8_t nbytes = (uint8_t)BT.available();
	while(nbytes--)
	{
		uint8_t data = BT.read();
		Serial1.write(data);
		
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
			_step = 0;
			break;
		}
	}
}

void BluetoothClass::update(uint8_t health, uint8_t status, uint8_t effect)
{
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
