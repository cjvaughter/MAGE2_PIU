/*******************************************************************************
* Name: XBee.cpp
* Description: DigiMesh API Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/

#include "XBee.h"

XBeeClass XBee;

void XBeeClass::init()
{
	XB.begin(38400);
	connected = false;
	nextTime = 0;
	_msgReady = false;
	_msg_index = 0;
	_busy = false;
	tx_bfr = new uint8_t[50];
	tx_data = new uint8_t[31];
	rx_data = new uint8_t[31];
	
	tx_bfr[0] = (Delimiter);
	tx_bfr[3] = (TX);
	tx_bfr[4] = (0); //No response
	tx_bfr[5] = (Coordinator0);
	tx_bfr[6] = (Coordinator1);
	tx_bfr[7] = (Coordinator2);
	tx_bfr[8] = (Coordinator3);
	tx_bfr[9] = (Coordinator4);
	tx_bfr[10] = (Coordinator5);
	tx_bfr[11] = (Coordinator6);
	tx_bfr[12] = (Coordinator7);
	tx_bfr[13] = (Coordinator8);
	tx_bfr[14] = (DefaultAddress16High);
	tx_bfr[15] = (DefaultAddress16Low);
	tx_bfr[16] = (0); //Maximum network hops
	tx_bfr[17] = (0); //No options
	api_mode();
	Debugger.out(XBeeLibrary, Initialized);
}

void XBeeClass::connect(uint16_t player_id, uint16_t device_id)
{
	tx_data[0] = Connect;
	tx_data[1] = (byte)(player_id>>8);
	tx_data[2] = (byte)player_id;
	tx_data[3] = (byte)(device_id>>8);
	tx_data[4] = (byte)device_id;
	Encode(5);
	Debugger.out(XBeeLibrary, MsgTX, "Connect attempt");
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
	tx_data[0] = Heartbeat;
	Encode(1);
	Debugger.out(XBeeLibrary, "Heartbeat");
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
	enter_at_mode();
	XB.print("ATAP 0\r");
	XB.flush();
	wait_for_cr();
	exit_at_mode();
}

void XBeeClass::api_mode()
{
	enter_at_mode();
	XB.print("ATAP 2\r");
	//XB.print("ATFR\r");
	XB.flush();
	wait_for_cr();
	exit_at_mode();
}

void XBeeClass::enter_at_mode()
{
	delay(10);
	XB.print("+++");
	XB.flush();
	wait_for_cr();
}

void XBeeClass::exit_at_mode()
{
	XB.write("ATCN\r");
	XB.flush();
	wait_for_cr();
}

void XBeeClass::wait_for_cr()
{
	if(XB.read() == 0x0D)
		return; //sweet
	
	uint8_t count = 10;
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
	if (data == Escape)
    {
        _escape = true;
        return;
    }
    if (_escape)
    {
        data ^= XOR;
        _escape = false;
    }
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
			_length = (uint8_t)(data << 8);
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
			if (_index == _length - RX_Header) _step = 255;
			break;
		case 255:
			_checksum = data;
			if (Check - _sum == _checksum)
			{
				rx_length = _index;
				_msgReady = true;
				rx_data[rx_length] = '\0';
				Debugger.out(XBeeLibrary, MsgRX, (char*)rx_data);
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
	
	byte offset = 18;
	for(byte i = 0; i < length; i++)
	{
		sum += tx_data[i];
		if (tx_data[i] == Delimiter || tx_data[i] == Escape || tx_data[i] == XON || tx_data[i] == XOFF)
		{
			tx_data[i] ^= XOR;
			tx_bfr[offset++] = (Escape);
		}
		tx_bfr[offset++] = tx_data[i];
	}
	tx_bfr[offset++] = ((byte)(Check - sum));
	
	XB.write(tx_bfr, offset);
	
	tx_data[length] = '\0';
	nextTime = currentTime + 4000;
}
