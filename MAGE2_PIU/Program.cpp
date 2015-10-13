#include <Arduino.h>
#include <Debugger.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <RGB.h>
#include <SD.h>
#include <Haptic.h>

#define SW_VER_MAJ 0
#define SW_VER_MIN 1

enum States
{
	Alive,
	Dead,
	Stunned,
};

byte state = Dead;
byte connected = true;
uint64_t currentTime = 0;
uint64_t nextTime = 0;        
			    
void setup()
{
	sei();
	
	//load sd settings
	
	Debugger.init();
	//XBee.init();
	//Bluetooth.init();
	//Ux.init();
	//MIRP.init();
	
	Debugger.out(MainProgram, Initialized);
}

void loop()
{	
	currentTime = millis();
	
	//XBee transactions 
	if(connected)
	{
		if(currentTime >= nextTime)
		{
			XBee.heartbeat();
			nextTime = currentTime + 2000;
		}
	}
	
	XBee.read();
	if(XBee.msgReady)
	{
		switch(XBee.rx_data[0])
		{	
			case Connect:
				state = Alive;
				connected = true;
				break;
			case DFU:
				Debugger.out(MainProgram, "Rebooting in DFU mode...");
				XBee.transparent_mode();
				cli();
				EIND = 1;
				asm volatile (
				"ldi r31, 0xFE\n"
				"ldi r30, 0x00\n"
				"mov r2,r31\n"
				"eijmp\n");	//set OTA flag
				break;
			default:
				Debugger.out(MainProgram, "Unrecognized message");
				break;
		}
		XBee.msgReady = false;
	}
	/*
	//Bluetooth transactions
	Bluetooth.read();
	if(Bluetooth.msgReady)
	{
		switch(Bluetooth.rx_data[0])
		{
			case 1:
				state = Alive;
				connected = true;
				lastTime = currentTime;
				break;
			case DFU:
				DEBUGGER("Received DFU command");
				state = DFU;
				XBee.transparent_mode();
				DEBUGGER("Resetting...");
				digitalWrite(A15, LOW);
				cli();
				EIND = 1;
				asm volatile (
				"ldi r31, 0xFE\n"
				"ldi r30, 0x00\n"
				"mov r2,r31\n"
				"eijmp\n");	//set OTA flag
				break;
			default:
				DEBUGGER("Unknown XBee transmission");
				break;
		}
		Bluetooth.msgReady = false;
	}*/
}
