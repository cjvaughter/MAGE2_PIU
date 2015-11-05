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
uint16_t player_id = 0xCCCC;
uint8_t team = 0;     			    
uint8_t lastDirection = NoDirection;
	
void Error(const char* message);
void reset(bool DFU = false);
void debug_programming_hook();
	
void setup()
{
	sei();
	Debugger.init();
	Serial2.begin(115200);
	Serial2.println("Program Started");
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
	
	Debugger.out(MainProgram, Initialized);
	RGB.setLed(Team, Red, B_100);
	XBee.connect(player_id, 0xEEEE);
}

void loop()
{	
	debug_programming_hook();
	
	currentTime = millis();
	
	//XBee transactions 
	XBee.run(currentTime);
	if(XBee.available())
	{
		switch(XBee.nextByte())
		{	
			case Connect:
				XBee.connected = true;
				RGB.setLed(Team, XBee.nextByte(), B_100);
				//Haptic.pulse(NoDirection, 1, 50, 50);
				//tell weapon
				break;
			case Disconnect:
				XBee.connected = false;
				RGB.setLed(All, NoColor);
				RGB.setLed(HealthBar, NoColor);
				Haptic.pulse(NoDirection, 1, 50, 50);
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
				Debugger.out(MainProgram, "Rebooting in DFU mode...");
				XBee.transparent_mode();
				reset(true);
				break;
			default:
				Debugger.out(MainProgram, "Unrecognized XBee message");
				RGB.setLed(Team, Red, B_100, Blink);
				XBee.discard();
				break;
		}
	}
	
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
				Debugger.out(MainProgram, "Unrecognized BT message");
				break;
		}
		Bluetooth.msgReady = false;
	}
	*/
	
	//UX
	Haptic.run(currentTime);
	RGB.run(currentTime);
}

void Error(const char* message)
{
	RGB.setLed(All, Red, B_60, Blink);
	RGB.setLed(Power, Red, B_100, Blink);
	Debugger.out(MainProgram, message);
	exit(1);
}

void reset(bool DFU)
{
	RGB.setBlinkRate();
	RGB.setLed(All, NoColor);
	RGB.setLed(Power, Red, B_100, Blink);
	if(DFU)
		RGB.setLed(Team, Green, B_60, Blink);
	else
		RGB.setLed(Team, Purple, B_100);
	
	cli();				    //Don't interrupt me
	
	if(DFU)
		MCUSR = _BV(EXTRF); //Set reset flag to External Reset (run bootloader)
	else
		//MCUSR = _BV(PORF);  //Set reset flag to Power On Reset (skip to main program)
		MCUSR = 0;		    //Clear reset flags (run bootloader [for debugging])
		
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

void debug_programming_hook()
{
	if(Serial2.available())
	{
		uint8_t nbytes = (uint8_t)Serial2.available();
		while(nbytes--)
		{
			if(Serial2.read() == 0x55)
			reset();
		}
	}
}
