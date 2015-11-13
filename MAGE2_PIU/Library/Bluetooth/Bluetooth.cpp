/*******************************************************************************
* Name: Bluetooth.cpp
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#include "Bluetooth.h"

BluetoothClass Bluetooth;

void BluetoothClass::init()
{
	BT.begin(115200); //factory default BAUD rate
	
	enter_at_mode();
	command("SN", "MAGE2_PIU"); //sets name
	command("SG", "2"); //sets to BT classic mode
	command("SA", "4"); //sets to pin code mode
	command("SP", "1234"); //sets to pin code mode
	command("SM", "0"); //sets to slave mode
	command("SY", "4"); //sets TX power to max
	exit_at_mode();
}

void BluetoothClass::enter_at_mode()
{
	BT.print("$$$\r");
	BT.flush();
	while(BT.read() != 0x0D);
}

void BluetoothClass::exit_at_mode()
{
	BT.print("---\r");
	BT.flush();
	while(BT.read() != 0x0D);
}

void BluetoothClass::command(const char* cmd, const char* data)
{
	BT.print(cmd);
	BT.print(",");
	BT.print(data);
	BT.print("\r");
	BT.flush();
	while(BT.read() != 0x0D);
}

void BluetoothClass::run()
{
	uint8_t nbytes = (uint8_t)BT.available();
	while(nbytes--)
	{
		uint8_t data = BT.read();
		
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
			switch(rx_func)
			{
			case 0x00: case 0x02: case 0xFF:
				_step = 255;
				break;
			case 0x01:
				_step = 2;
				break;
			case 0x03:
				_step = 5;
				break;
			}
			break;
		case 2:
			device_id = data<<8;
			_step++;
			break;
		case 3:
			device_id |= data;
			_step++;
			break;
		case 4:
			color = data;
			_step = 255;
			break;
		case 5:
			device_status = data;
			_step++;
			break;
		case 255:
			_checksum = data;
			if (Check - _sum == _checksum)
			{
				if(rx_func != ACK)
				{
					msgReady = true;
					ack();
				}
			}
			_step = 0;
			break;
		}
	}
}

void BluetoothClass::write()
{
	uint8_t sum = tx_func;
	switch(tx_func)
	{
		case 0x03:
			BT.write(device_status);
			sum += device_status;
			break;
	}
	BT.write(Check - sum);
}

void BluetoothClass::ack()
{
	BT.write(BTDelimiter);
	BT.write(ACK);
	BT.write(0x00);
}
