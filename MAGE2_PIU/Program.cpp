#include "Arduino.h"
#include "Bluetooth.h"

bool XBMsgRx = false;
bool XBMsgStart = false;
bool BTMsgRx = false;
bool BTMsgStart = false;

void CheckXB();
void CheckBT();
void ProcessMessages();
void UpdateDisplay();

void setup()
{
	//XB.begin(9600);
	Bluetooth.setPort(&Serial);
}

void loop()
{
	CheckXB();
	CheckBT();
	ProcessMessages();
	UpdateDisplay();
}

void CheckXB()
{
	//if (XB.available() > 0)
	{
		//read bytes into packet
		//set flags
	}
}

void CheckBT()
{
	//if (BT.available() > 0)
	{
		//read bytes into packet
		//set flags
	}
}

void ProcessMessages()
{
	//use packets to alter state
	//send any necessary responses
}

void UpdateDisplay()
{
	//do light stuff here
}
