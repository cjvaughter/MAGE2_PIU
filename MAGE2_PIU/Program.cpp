#include <Arduino.h>
#include <Bluetooth.h>
#include <Constants.h>
#include <Haptic.h>
#include <MIRP2.h>
#include <RGB.h>
#include <Settings.h>
#include <XBee.h>

uint8_t state = Dead;
uint64_t currentTime = 0;
uint16_t player_id = 0;
uint8_t team = 0;     			    
uint8_t lastDirection = NoDirection;

uint64_t nextTime = 0;
	
void Error(const char* message);
void reset(boolean DFU = false);
	
void setup()
{
	Serial.begin(9600);
	
	RGB.init();
	XBee.init();
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
	Haptic.init();
	MIRP2.init();
	
	//if(!Bluetooth.init())
		//Error("Could not communicate with Bluetooth module.");
}

void loop()
{
	currentTime = millis();
	/*
	if(currentTime > nextTime)
	{
		nextTime = currentTime + 1500;
		if(lastDirection > Right)
			lastDirection = Front;
		else
			lastDirection++;
		RGB.setDirection(Red, lastDirection, currentTime);
	}
	*/
	
	if(!XBee.connected)
	{
		XBee.connect(0xCCCC, 0xEEEE);
		delay(1000);
	}
	
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
				//tell weapon
				break;
			case Disconnect:
				XBee.connected = false;
				RGB.setLed(All, NoColor);
				//tell weapon
				break;
			case Health:
				RGB.setHealth(XBee.nextByte());
				break;
			case State:
				state = XBee.nextByte();
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
						Haptic.pulse(NoDirection, 1, 120);
						break;
					case Damaged:
						RGB.setDirection(NoColor, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 1, 800);
						break;
					case Stunned:
						RGB.setDirection(Blue, lastDirection, currentTime, true);
						Haptic.pulse(lastDirection, 4, 120, 80);
						break;
					case Healed:
						RGB.setDirection(Green, lastDirection, currentTime);
						Haptic.pulse(lastDirection, 2, 300, 100);
						break;
					case Dead:
						RGB.setLed(HealthBar, Red);
						Haptic.pulse(NoDirection, 1, 1500);
						break;
				}
				RGB.doEffect(currentTime);
				lastDirection = NoDirection;
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
	
	//Bluetooth transactions
	Bluetooth.run();
	if(Bluetooth.msgReady)
	{
		switch(Bluetooth.rx_func)
		{
			//process messages here
			case 1:
				Bluetooth.connected = true;
				//XBee.connect(player_id, Bluetooth.device_id);
				RGB.setLed(HealthBar, Blue);
				break;
			case 0x00:
				//update heartbeat time
				RGB.setLed(Team, Green);
				delay(50);
				RGB.setLed(Team, NoColor);
				break;
			case 0xFF:
				//do nothing
				break;
			default:
				RGB.setLed(Team, Red);
				delay(100);
				RGB.setLed(Team, NoColor);
				break;
		}
		Bluetooth.ack();
		Bluetooth.msgReady = false;
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
	Haptic.run(currentTime);
	RGB.run(currentTime);
}

void Error(const char* message)
{
	Serial.println(message);
	while(1)
	{
		RGB.setLed(All, Red, B_60);
		RGB.setLed(Power, Red, B_100);
		delay(300);
		RGB.setLed(All, NoColor);
		RGB.setLed(Power, NoColor);
		delay(300);
	}
}

void reset(boolean DFU)
{
	RGB.setBlinkRate();
	RGB.setLed(All, NoColor);
	RGB.setLed(Power, Red, B_100, Blink);
	if(DFU)
		RGB.setLed(Team, Green, B_60, Blink);
	
	cli();				    //Don't interrupt me
	
	if(DFU)
		MCUSR = _BV(EXTRF); //Set reset flag to External Reset (run bootloader)
	else
		MCUSR = _BV(PORF);  //Set reset flag to Power On Reset (skip to main program)
		
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
