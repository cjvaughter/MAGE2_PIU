/*******************************************************************************
* Name: XBee.cpp
* Description: XBEE control library for use with the MAGE 2 PIU
* Author: CJ Vaughter & Jacob Dixon
* Created: 9/21/2015 2:26:59 PM
* Updated: 9/29/2015 Rev. A - Jacob Dixon
*******************************************************************************/

#include "XBee.h"

XBeeClass XBee;

void XBeeClass::CurrentMessage()
{
	//From C#:
	/*
	get
            {
                MAGEMsg temp = _currentMessage;
                _currentMessage = null;
                Ready = false;
                return temp;
            }
            private set
            {
                _currentMessage = value;
                Ready = true;
            }
	*/
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
			if (data == Delimiter) _step++;
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
			_api = data;
			_sum = data;
			if (_api != RX) _step = 0;
			else _step++;
			break;
		case 4:
			_address = ((long)data) << 56;
			_sum += data;
			_step++;
			break;
		case 5:
			_address |= ((long)data) << 48;
			_sum += data;
			_step++;
			break;
		case 6:
			_address |= ((long)data) << 40;
			_sum += data;
			_step++;
			break;
		case 7:
			_address |= ((long)data) << 32;
			_sum += data;
			_step++;
			break;
		case 8:
			_address |= ((long)data) << 24;
			_sum += data;
			_step++;
			break;
		case 9:
			_address |= ((long)data) << 16;
			_sum += data;
			_step++;
			break;
		case 10:
			_address |= ((long)data) << 8;
			_sum += data;
			_step++;
			break;
		case 11:
			_address |= ((long)data);
			_sum += data;
			_step++;
			break;
		case 12:
			_address16 = (uint8_t)(data << 8);
			_sum += data;
			_step++;
			break;
		case 13:
			_address16 |= data;
			_sum += data;
			_step++;
			break;
		case 14: //Ignore options
			_sum += data;
			_step++;
			break;
		case 15:
			_data = new byte[_length - RX_Header];
			_index = 0;
			_data[_index++] = data;
			_sum += data;
			_step++;
			break;
		default:
			_data[_index++] = data;
			_sum += data;
			if (_index == _length - RX_Header) _step = 255;
			break;
		case 255:
			_checksum = data;
			if (Check - _sum == _checksum)
			{
				CurrentMessage = new MAGEMsg(_address, _data);
			}
			_step = 0;
			break;
    }
}

void XBeeClass::Encode(uint8_t[] data)
{
	//From C#:
	/*
	byte sum = 0;
	List<byte> data = new List<byte>();
	data.Add(Delimiter);
	data.Add((byte)((TX_Header + msg.Data.Length) >> 8));
	data.Add((byte)(TX_Header + msg.Data.Length));
	data.Add(TX);
	data.Add(0); //No response
	data.Add((byte)(msg.Address >> 56));
	data.Add((byte)(msg.Address >> 48));
	data.Add((byte)(msg.Address >> 40));
	data.Add((byte)(msg.Address >> 32));
	data.Add((byte)(msg.Address >> 24));
	data.Add((byte)(msg.Address >> 16));
	data.Add((byte)(msg.Address >> 8));
	data.Add((byte)(msg.Address));
	data.Add(DefaultAddress16High);
	data.Add(DefaultAddress16Low);
	data.Add(0); //Maximum network hops
	data.Add(0); //No options

	foreach (byte b in msg.Data)
	{
		data.Add(b);
	}

	for (int i = 3; i < data.Count; i++) sum += data[i];
	data.Add((byte)(Check - sum));

	for (int i = 3; i < data.Count - 1; i++)
	{
		if (data[i] == Delimiter || data[i] == Escape || data[i] == XON || data[i] == XOFF)
		{
			data[i] ^= XOR;
			data.Insert(i, Escape);
		}
	}

	return data.ToArray();
	*/
}

void XBeeClass::MAGEMsg(long address, uint8_t[] data)
{
	Address = address;
	Data = data;
}

 