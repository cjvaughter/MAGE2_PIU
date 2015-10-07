#include <Arduino.h>
#include <Bluetooth.h>
#include <XBee.h>

enum States
{
	Alive,
	Dead,
	Stunned,
	Disconnected,
	
	DFU = 0xFF
};

byte state = Disconnected;
uint64_t currentTime = 0;
uint64_t lastTime = 0;
		            
void setup()
{
	cli();
	XBee.init(&Serial1);
	//Bluetooth.init(&SerialX);
	//Ux.init();
	//MIRP.init();
	sei();
}

void loop()
{
	currentTime = millis();
	
	//XBee transactions 
	XBee.read();
	if(XBee.msgReady)
	{
		switch(XBee.rx_data[0])
		{
			case DFU:
				state = DFU;
				break;
		}
		XBee.msgReady = false;
	}
	if(currentTime - lastTime >= 2000)
	{
		XBee.heartbeat();
		lastTime = currentTime; 
	}
	
	//Bluetooth transactions
	
	//Game state
	switch(state)
	{
		case Alive:
			break;
		case Dead:
			break;
		case Stunned:
			break;
		case Disconnected:
			break;
		case DFU:
			cli();
			EIND = 0x01;	  //load jump address
			asm volatile (
			"ldi r31, 0xFE\n" //...
			"ldi r30, 0x00\n" //...
			"ser r16\n"	  //set OTA flag
			"eijmp\n");		  //jump to bootloader
			break;
		default:
			break;
	}
}
