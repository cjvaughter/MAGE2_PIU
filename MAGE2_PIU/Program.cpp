#include <Arduino.h>
#include <Constants.h>
#include <Debugger.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <RGB.h>
#include <Settings.h>
#include <Haptic.h>

uint8_t state = Dead;
uint64_t currentTime = 0;
uint16_t player_id = 0;
uint8_t team = 0;     			    
uint8_t lastDirection = Front;
	
void Error(const char* message);
	
void setup()
{
	Debugger.init();
	RGB.init();
	/*
	if(Settings.init()) 
	{
		if(Settings.read())
		{
			player_id = Settings.player_id;
			Debugger.enabled = Settings.debugEnabled;
			if(player_id == 0)
				Error("Invalid player ID!");
		}
		else
		{
			Error("Could not read settings!");
		}
	}
	else
	{
		Error("No SD card!");
	}
	*/
	XBee.init();
	//Bluetooth.init();
	Haptic.init();
	//MIRP.init();
	
	//Debugger.out(MainProgram, Initialized);
	XBee.connect(player_id, 0xEEEE);
}

void loop()
{	
	currentTime = millis();
	
	//XBee transactions 
	XBee.run(currentTime);
	if(XBee.available())
	{
		switch(XBee.nextByte())
		{	
			case Connect:
				XBee.connected = true;
				team = XBee.nextByte();
				Haptic.pulse(NoDirection, 2, 200, 200);
				//set team led
				break;
			case Disconnect:
				XBee.connected = false;
				RGB.setLed(All, NoColor);
				break;
			case Health:
				RGB.setHealth(XBee.nextByte());
				break;
			case State:
				state = XBee.nextByte();
				//if (XBee.nextByte() == 1) lastDirection = NoDirection;
				break;
			case Effect:
				RGB.setEffect(XBee.nextByte());
				break;
			case Update:
				switch(state)
				{
					case Alive:
						RGB.showHealth();
						Haptic.pulse(NoDirection, 5, 150, 150);
						break;
					case Damaged:
						RGB.setDirection(NoColor, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 1, 100, 100);
						break;
					case Stunned:
						RGB.setDirection(Blue, lastDirection, currentTime, true);
						Haptic.pulse(lastDirection, 4, 50, 50);
						break;
					case Healed:
						RGB.setDirection(Green, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 2, 25, 125);
						break;
					case Dead:
						RGB.setLed(HealthBar, Red);
						Haptic.pulse(NoDirection, 1, 400, 100);
						break;
				}
				RGB.doEffect(currentTime);
				//update weapon
				break;
			case DFU:
				Debugger.out(MainProgram, "Rebooting in DFU mode...");
				RGB.setBlinkRate();
				RGB.setLed(All, NoColor);
				RGB.setLed(Power, Red, B_60, Blink);
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
				XBee.discard();
				break;
		}
	}
	
	//Bluetooth transactions
	Bluetooth.run();
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

void Error(const char* message)
{
	RGB.setLed(All, NoColor);
	RGB.setLed(All, Red, B_60, Blink);
	RGB.setLed(Power, Red, B_100, Blink);
	Debugger.out(MainProgram, message);
	exit(1);
}
