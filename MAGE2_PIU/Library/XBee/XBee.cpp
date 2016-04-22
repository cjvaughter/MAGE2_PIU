/*******************************************************************************
* Name: XBee.cpp
* Description: DigiMesh API Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/

#include "XBee.h"

XBeeClass XBee;

void XBeeClass::init(uint64_t address)
{
	LOG("Initializing...");

	XB.begin(38400);
	coordinator = address;
	connected = false;
	nextTime = 0;
	_msgReady = false;
	_msg_index = 0;
	_busy = false;
	
	tx_bfr[0] = (Delimiter);
	tx_bfr[3] = (TX);
	tx_bfr[4] = (0); //No response
	tx_bfr[5] = (byte)(coordinator >> 56);
	tx_bfr[6] = (byte)(coordinator >> 48);
	tx_bfr[7] = (byte)(coordinator >> 40);
	tx_bfr[8] = (byte)(coordinator >> 32);
	tx_bfr[9] = (byte)(coordinator >> 24);
	tx_bfr[10] = (byte)(coordinator >> 16);
	tx_bfr[11] = (byte)(coordinator >> 8);
	tx_bfr[12] = (byte)(coordinator);
	tx_bfr[13] = (DefaultAddress16High);
	tx_bfr[14] = (DefaultAddress16Low);
	tx_bfr[15] = (0); //Maximum network hops
	tx_bfr[16] = (0); //No options
	
	SendChecksum = 0;
	for(int i = 3; i < 15; i++)
	{
		SendChecksum += tx_bfr[i];
	}
	//api_mode();

	LOG("Success!");
}

void XBeeClass::connect(uint16_t player_id, uint16_t device_id)
{
	LOG("Connect request");
	tx_data[0] = Connect;
	tx_data[1] = (byte)(player_id>>8);
	tx_data[2] = (byte)player_id;
	tx_data[3] = (byte)(device_id>>8);
	tx_data[4] = (byte)device_id;
	Encode(5);
}

boolean XBeeClass::available()
{
	return _msgReady;
}

void XBeeClass::discard()
{
	_msgReady = false;
	_msg_index = 0;
}

void XBeeClass::heartbeat()
{
	//LOG("Heartbeat"); //annoying
	tx_data[0] = Heartbeat;
	Encode(1);
}

void XBeeClass::run(uint64_t time)
{
	currentTime = time;
	if(connected)
	{
		if(currentTime >= nextTime && !_busy)
		{
			heartbeat();
		}
	}
	
	uint8_t nbytes = (uint8_t)XB.available();
	while(nbytes--) Decode(XB.read());
}

uint8_t XBeeClass::nextByte()
{
	uint8_t data = rx_data[_msg_index++];
	if(_msg_index >= rx_length)
	{
		_msg_index = 0;
		_msgReady = false;
	}
	return data;
}

void XBeeClass::transparent_mode()
{
	LOG("Entering transparent mode");
	enter_at_mode();
	XB.print("ATAP 0\r");
	XB.flush();
	wait_for_cr();
	exit_at_mode();
}

void XBeeClass::api_mode()
{
	LOG("Entering API mode");
	enter_at_mode();
	XB.print("ATAP 1\r");
	XB.flush();
	wait_for_cr();
	exit_at_mode();
}

void XBeeClass::enter_at_mode()
{
	LOG("Entering AT mode");
	delay(100);
	XB.print("+++");
	XB.flush();
	wait_for_cr();
}

void XBeeClass::exit_at_mode()
{
	LOG("Exiting AT mode");
	XB.write("ATCN\r");
	XB.flush();
	wait_for_cr();
}

void XBeeClass::wait_for_cr()
{
	if(XB.read() == 0x0D)
		return; //sweet
	
	uint8_t count = 100;
	while(count--)
	{
		delay(1);
		if(XB.read() == 0x0D)
			return; //not bad
	}
	return; //you tried
}

void XBeeClass::Decode(uint8_t data)
{
	switch (_step)
	{
		case 0:
			if (data == Delimiter)
			{
				_index = 0;
				_step++;
				_busy = true;
			}
			break;
		case 1:
			_length = (uint16_t)(data << 8);
			_step++;
			break;
		case 2:
			_length |= data;
			_step++;
			break;
		case 3:
			if (data != RX)
			{
				_step = 0;
				_busy = false;
			}
			else
			{
				_sum = data;
				_step++;
			}
			break;
		case 4: case 5: case 6: case 7:
		case 8: case 9: case 10: case 11:
		case 12: case 13: case 14:
			_sum += data;
			_step++;
			_msgReady = false; //if you haven't read the message by now, you missed it
			break;
		default:
			rx_data[_index++] = data;
			_sum += data;
			if (_index >= _length - RX_Header) _step = 255;
			break;
		case 255:
			_checksum = data;				
			if (Check - _sum == _checksum)
			{
				rx_length = _index;
				if(!connected && rx_data[0] != Connect) _msgReady = false;
				else _msgReady = true;
				rx_data[rx_length] = '\0';
			}
			else
			{
				LOG("BAD CHECKSUM");
			}
			_step = 0;
			_busy = false;
			nextTime = 0;
			break;
    }
}

void XBeeClass::Encode(uint8_t length)
{
	byte sum = SendChecksum;
	
	uint16_t temp = TX_Header + length;
	tx_bfr[1] = ((byte)((temp) >> 8));
	tx_bfr[2] = ((byte)(temp));
	
	byte offset = 17;
	for(byte i = 0; i < length; i++)
	{
		sum += tx_data[i];
		tx_bfr[offset++] = tx_data[i];
	}
	tx_bfr[offset++] = ((byte)(Check - sum));
	
	XB.write(tx_bfr, offset);
	
	tx_data[length] = '\0';
	nextTime = currentTime + 2000;
}
