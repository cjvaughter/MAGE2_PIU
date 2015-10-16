#include <Arduino.h>
#include <Debugger.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <RGB.h>
#include <Settings.h>
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
boolean connected = true;
uint64_t currentTime = 0;
uint64_t nextTime = 0;
uint16_t player_id = 0;     
			    
void setup()
{
	RGB.init();
	Debugger.init();
	
	if(Settings.init()) 
	{
		Settings.read();
		player_id = Settings.player_id;
		Debugger.enabled = Settings.debugEnabled;
	}
	else
	{
		RGB.setLed(All, Red);
		exit(1);
	}
	
	
	//XBee.init();
	//Bluetooth.init();
	Haptic.init();
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
				Haptic.pulse(2, 50, 100);
				break;
			case DFU:
				Debugger.out(MainProgram, "Rebooting in DFU mode...");
				RGB.setBlinkRate();
				RGB.setLed(All, NoColor);
				RGB.setLed(Power, Red, Blink);
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
				Debugger.out(MainProgram, "Unrecognized XBee message");
				break;
		}
		XBee.msgReady = false;
	}
	
	//Bluetooth transactions
	Bluetooth.read();
	if(Bluetooth.msgReady)
	{
		switch(Bluetooth.rx_func)
		{
			//process messages here
			case 1:
				XBee.connect(player_id, Bluetooth.device_id);
				break;
			default:
				Debugger.out(MainProgram, "Unrecognized BT message");
				break;
		}
		Bluetooth.msgReady = false;
	}
	
	//UX
	Haptic.run(currentTime);
	RGB.run(currentTime);
}