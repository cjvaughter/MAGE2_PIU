#include <Arduino.h>
#include <Bluetooth.h>
#include <Constants.h>
#include <Haptic.h>
#include <MIRP2.h>
#include <RGB.h>
#include <Settings.h>
#include <XBee.h>

uint8_t state = Alive;
uint64_t currentTime = 0;
uint16_t player_id = 0xCCCC;
uint8_t team = 0;     			    
uint8_t lastDirection = NoDirection;
	
void Error(const char* message);
void reset(boolean DFU = false);
	
void setup()
{
	sei();
	RGB.init();
	/*
	if(Settings.init()) 
	{
		if(Settings.read())
		{
			player_id = Settings.player_id;
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

	//Haptic.init();
	MIRP2.init();
	//XBee.init();
	if(Bluetooth.init()) RGB.setLed(HealthBar, Blue);
}

void loop()
{
	//currentTime = millis();
	/*
	//XBee transactions 
	XBee.run(currentTime);
	if(XBee.available())
	{
		switch(XBee.nextByte())
		{	
			case Connect:
				XBee.connected = true;
				team = XBee.nextByte();
				RGB.setLed(Team, team);
				//Haptic.pulse(NoDirection, 1, 50, 50);
				//tell weapon
				break;
			case Disconnect:
				XBee.connected = false;
				RGB.setLed(All, NoColor);
				RGB.setLed(HealthBar, NoColor);
				//Haptic.pulse(NoDirection, 1, 50, 50);
				//tell weapon
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
						RGB.setLed(Team, team);
						Haptic.stop();
						break;
					case Damaged:
						lastDirection++;
						RGB.setDirection(NoColor, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 1, 1000);
						break;
					case Stunned:
						lastDirection++;
						RGB.setDirection(Blue, lastDirection, currentTime, true);
						Haptic.pulse(lastDirection, 10, 1000, 500);
						break;
					case Healed:
						lastDirection++;
						RGB.setDirection(Green, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 2, 400, 100);
						break;
					case Dead:
						RGB.setLed(HealthBar, Red);
						Haptic.pulse(NoDirection, 1, 2000);
						break;
				}
				RGB.doEffect(currentTime);
				if(lastDirection >= Right) lastDirection = NoDirection;
				//update weapon
				break;
			case DFU:
				XBee.transparent_mode();
				reset(true);
				break;
			default:
				RGB.setLed(Team, Red, Blink);
				XBee.discard();
				break;
		}
		if(!XBee.available())
			XBee.heartbeat();
	}
	*/
	//Bluetooth transactions
	/*
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
				break;
		}
		Bluetooth.msgReady = false;
	}
	*/
	if(Serial2.available())
	{
		RGB.setLed(Team, Yellow);
		if(Serial2.read() == 0x42) RGB.setLed(Team, Blue);
	}
	else
	{
		Serial2.println("Dickbutt");
		delay(500);
	}
	
	if(MIRP2.msgReady && state != Dead)
	{
		cli();
		XBee.tx_data[0] = Spell_RX;
		XBee.tx_data[1] = MIRP2.data[0];
		XBee.tx_data[2] = MIRP2.data[1];
		XBee.tx_data[3] = MIRP2.data[2];
		XBee.tx_data[4] = MIRP2.data[3];
		XBee.tx_data[5] = MIRP2.data[4];
		lastDirection = MIRP2.direction;
		MIRP2.msgReady = false;
		sei();
		XBee.Encode(6);
	}
	
	//UX
	//Haptic.run(currentTime);
	//RGB.run(currentTime);
}

void Error(const char* message)
{
	RGB.setLed(All, Red, B_60, Blink);
	RGB.setLed(Power, Red, B_60, Blink);
	exit(1);
}

void reset(boolean DFU)
{
	RGB.setBlinkRate();
	RGB.setLed(All, NoColor);
	RGB.setLed(Power, Red, B_60, Blink);
	if(DFU)
		RGB.setLed(Team, Green, B_60, Blink);
	else
		RGB.setLed(Team, Purple);
	
	cli();				    //Don't interrupt me
	
	if(DFU)
		MCUSR = _BV(EXTRF); //Set reset flag to External Reset (run bootloader)
	else
		MCUSR = _BV(PORF);  //Set reset flag to Power On Reset (skip to main program)
		//MCUSR = 0;		    //Clear reset flags (run bootloader [for debugging])
		
	EIND = 1;			    //Jump address high
	asm volatile
	(
		"ldi r31, 0xFE\n"   //Jump address mid
		"ldi r30, 0x00\n"   //Jump address low
		"mov r2, %0\n"      //Set (or clear) OTA flag
		"eijmp\n"		    //Jump to bootloader
		:: "r" (DFU)
	);
}
