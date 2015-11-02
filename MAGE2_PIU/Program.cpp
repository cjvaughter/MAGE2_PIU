#include <Arduino.h>
#include <Debugger.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <RGB.h>
#include <Settings.h>
#include <Haptic.h>
#include <avr/wdt.h>

#define SW_VER_MAJ 0
#define SW_VER_MIN 1

enum States
{
	Alive,
	Dead,
	Stunned,
};

uint8_t state = Dead;
uint8_t health = 0;
boolean connected = false;
uint64_t currentTime = 0;
uint64_t nextTime = 0;
uint16_t player_id = 0;     			    
uint8_t msg_index = 0;

boolean d_pressed = false;
boolean s_pressed = false;
boolean h_pressed = false;
uint8_t unique = 0;
	
void Error(const char* message);	
	
void setup()
{
	Debugger.init();
	RGB.init();
	
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

	XBee.init();
	//Bluetooth.init();
	Haptic.init();
	//MIRP.init();
	
	Debugger.out(MainProgram, Initialized);
	XBee.connect(player_id, 0xEEEE);
	
	DDRK |= 0xE0;
	PORTK |= 0xE0;
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
		switch(XBee.rx_data[msg_index++])
		{	
			case Connect:
				health = 0;
				RGB.setHealth(0);
				connected = true;
				Haptic.pulse(2, 50, 100);
				msg_index++;
				break;
			case Disconnect:
				RGB.setHealth(255);
				connected = false;
				break;
			case Health:
				health = XBee.rx_data[msg_index++];
				RGB.setHealth(health);
				break;
			case State:
				state = XBee.rx_data[msg_index++];
				switch(state)
				{
					case Alive:
						RGB.setHealth(health);
						//solid team led
						//tell weapon
						break;
					case Dead:
						//team led off
						//tell weapon
						break;
					case Stunned:
						RGB.setLed(HealthBar, Blue, Normal, B_50);
						//blink team led
						//tell weapon
						break;
				}
				//tell weapon our state
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
				msg_index = 255;
				break;
		}
		if(msg_index >= XBee.rx_length)
		{
			msg_index = 0;
			XBee.msgReady = false;
		}
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
	
	if((PINK & 0x20) == 0 && !d_pressed)
	{
		delay(5);
		if((PINK & 0x20) == 0)
		{
			XBee.tx_data[0] = SentSpell;
			XBee.tx_data[1] = 0x00;
			XBee.tx_data[2] = unique;
			XBee.Encode(3);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Sent");
			XBee.tx_data[0] = ReceivedSpell;
			XBee.tx_data[1] = 0xCC;
			XBee.tx_data[2] = 0xCC;
			XBee.tx_data[3] = 0x00;
			XBee.tx_data[4] = unique;
			XBee.Encode(5);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Received");
			d_pressed = true;
			unique++;
		}
	}
	else if(d_pressed && ((PINK & 0x20) != 0))
	{
		d_pressed = false;
	}
	
	if((PINK & 0x40) == 0 && !s_pressed)
	{
		delay(5);
		if((PINK & 0x40) == 0)
		{
			XBee.tx_data[0] = SentSpell;
			XBee.tx_data[1] = 0x02;
			XBee.tx_data[2] = unique;
			XBee.Encode(3);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Sent");
			XBee.tx_data[0] = ReceivedSpell;
			XBee.tx_data[1] = 0xCC;
			XBee.tx_data[2] = 0xCC;
			XBee.tx_data[3] = 0x02;
			XBee.tx_data[4] = unique;
			XBee.Encode(5);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Received");
			s_pressed = true;
			unique++;
		}
	}
	else if(s_pressed && ((PINK & 0x40) != 0))
	{
		s_pressed = false;
	}
	
	if((PINK & 0x80) == 0 && !h_pressed)
	{
		delay(5);
		if((PINK & 0x80) == 0)
		{
			XBee.tx_data[0] = SentSpell;
			XBee.tx_data[1] = 0x04;
			XBee.tx_data[2] = unique;
			XBee.Encode(3);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Sent");
			XBee.tx_data[0] = ReceivedSpell;
			XBee.tx_data[1] = 0xCC;
			XBee.tx_data[2] = 0xCC;
			XBee.tx_data[3] = 0x04;
			XBee.tx_data[4] = unique;
			XBee.Encode(5);
			Debugger.out(XBeeLibrary, MsgTX, "Spell Received");
			h_pressed = true;
			unique++;
		}
	}
	else if(h_pressed && ((PINK & 0x80) != 0))
	{
		h_pressed = false;
	}
}

void Error(const char* message)
{
	RGB.setLed(All, Red, Blink, B_50);
	Debugger.out(MainProgram, message);
	exit(1);
}