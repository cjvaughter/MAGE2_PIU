/*******************************************************************************
* Name: XBee.cpp
* Description: XBEE control library for use with the MAGE 2 PIU
* Author: CJ Vaughter & Jacob Dixon
* Created: 9/21/2015 2:26:59 PM
* Updated: 9/29/2015 Rev. A - Jacob Dixon
*******************************************************************************/

#include "XBee.h"

XBeeClass XBee;

//public
uint8_t* XBeeClass::rx_data;
uint8_t* XBeeClass::tx_bfr;
uint8_t* XBeeClass::tx_data;
uint8_t XBeeClass::rx_length;
uint8_t XBeeClass::tx_length;
bool XBeeClass::msgReady;

//private
HardwareSerial* XBeeClass::_port;
uint8_t XBeeClass::_step;
uint16_t XBeeClass::_length;
uint8_t XBeeClass::_checksum;
bool XBeeClass::_escape;
uint8_t XBeeClass::_sum;
uint8_t XBeeClass::_index;

void XBeeClass::init(HardwareSerial* port)
{
	_port = port;
	_port->begin(38400);
	msgReady = false;
	tx_bfr = new uint8_t[51];
	tx_data = new uint8_t[32];
	rx_data = new uint8_t[32];
	
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
}

void XBeeClass::run()
{
	if(tx_length != 0)
	{
		Encode();
	}
	read();
}

void XBeeClass::heartbeat()
{
	tx_data[0] = 0;
	tx_length = 1;
	Encode();
}

void XBeeClass::read()
{
	uint8_t nbytes = (uint8_t)_port->available();
	while(nbytes--) Decode(_port->read());
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
			if (data != RX) _step = 0;
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
			msgReady = false; //if you haven't read the message by now, you missed it
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
				msgReady = true;
			}
			_step = 0;
			break;
    }
}

void XBeeClass::Encode()
{
	byte sum = SendChecksum;
	
	uint16_t temp = TX_Header + tx_length;
	tx_bfr[1] = ((byte)((temp) >> 8));
	tx_bfr[2] = ((byte)(temp));
	
	byte offset = 18;
	for(byte i = 0; i < tx_length; i++)
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
	
	_port->write(tx_bfr, offset);
	tx_length = 0;
}

 