#include <Arduino.h>
#include <Bluetooth.h>
#include <Constants.h>
#include <Haptic.h>
#include <MIRP2.h>
#include <RGB.h>
#include <Settings.h>
#include <XBee.h>

uint8_t state = Dead;
uint8_t health = 0;
uint8_t effect = 0;
uint64_t currentTime = 0;
uint16_t player_id = 0;
uint8_t team = Red;     			    
uint8_t lastDirection = NoDirection;
boolean connectionRequest = false;

uint64_t nextTime = 0;

void changeWeapon();	
void Error(const char* message, uint8_t color = Red);
void reset(boolean DFU = false);
void testMode();
	
void setup()
{
	Serial.begin(9600);
	RGB.init();
	
	if(Settings.init()) 
	{
		if(Settings.read())
		{
			player_id = Settings.player_id;
			if(player_id == 0)
				Error("Invalid player ID!");
			if(Settings.coordinatorAddress == 0)
				Error("Invalid coordinator address!");
		}
		else
		{
			Error("Could not read settings!");
		}
	}
	else
	{
		//Error("No SD card!");
		testMode();
	}
	
	XBee.init(Settings.coordinatorAddress);
	
	Haptic.init();
	MIRP2.init();
	
	if(!Bluetooth.init())
		Error("Could not communicate with Bluetooth module.", Blue);
	
	delay(250);
}

void loop()
{
	currentTime = millis();

	if(connectionRequest)
	{
		if(currentTime >= nextTime)
		{
			XBee.connect(player_id, Bluetooth.device_id);
			nextTime = currentTime + 1000;
		}
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
				RGB.setLed(Team, team, B_100);
				connectionRequest = false;
				if(!Bluetooth.connected)
				{
					Bluetooth.device_id = 0xFFFF;
					changeWeapon();
				}
				break;
			case Disconnect:
				XBee.connected = false;
				RGB.setLed(All, NoColor);
				connectionRequest = false;
				delay(1000);
				break;
			case Health:
				health = XBee.nextByte();
				RGB.setHealth(health);
				break;
			case State:
				state = XBee.nextByte();
				break;
			case Effect:
				effect = XBee.nextByte();
				RGB.setEffect(effect);
				RGB.doEffect();
				break;
			case Update:
				switch(state)
				{
					case Alive:
						if(effect == NoColor)
						{
							RGB.showHealth();
						}
						else
						{
							
						}
						break;
					case Damaged:
						RGB.setDirection(Red, lastDirection, currentTime);
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
				lastDirection = NoDirection;
				Bluetooth.update(health, state, effect);
				break;
			case DFU:
				XBee.transparent_mode();
				reset(true);
				break;
			default:
				//unrecognized message
				RGB.setLed(Team, Red, B_100, Blink);
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
			case BTHeartbeat:
				Bluetooth.nextHeartbeat = currentTime + 9000;
				Bluetooth.ack();
				break;
			case BTConnect:
				Bluetooth.connected = true;
				Bluetooth.device_id = (((uint16_t)Bluetooth.rx_data[0])<<8) | Bluetooth.rx_data[1];
				Bluetooth.ack();
				delay(50);
				Bluetooth.confirmConnection(player_id);
				
				if(XBee.connected)
					changeWeapon();
				else
				{
					connectionRequest = true;	
					RGB.setLed(HealthBar, Bluetooth.rx_data[2]);
					delay(80);
					RGB.setLed(HealthBar, NoColor);
					delay(80);
					RGB.setLed(HealthBar, Bluetooth.rx_data[2]);
					delay(80);
					RGB.setLed(HealthBar, NoColor);
				}
				
				Bluetooth.nextHeartbeat = currentTime + 3000;
				break;
			case BTSpell_TX:
				XBee.tx_data[0] = Spell_TX;
				XBee.tx_data[1] = Bluetooth.rx_data[0];
				Bluetooth.ack();
				XBee.Encode(2);
				break;
			case BTUpdate:
				//Do nothing for now
				//(destructible devices will be in a future update)
				Bluetooth.ack();
				break;
			default:
				//unrecognized message
				break;
		}
		Bluetooth.msgReady = false;
	}
	if(Bluetooth.connected)
	{
		if(currentTime >= Bluetooth.nextHeartbeat)
		{
			Bluetooth.connected = false;
			Bluetooth.device_id = 0xFFFF;
			changeWeapon();
		}
	}

	if(MIRP2.msgReady)
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

void changeWeapon()
{
	XBee.tx_data[0] = ChangeWeapon;
	XBee.tx_data[1] = (byte)(Bluetooth.device_id >> 8);
	XBee.tx_data[2] = (byte)(Bluetooth.device_id);
	XBee.Encode(3);
}

void Error(const char* message, uint8_t color)
{
	Serial.println(message);
	while(1)
	{
		RGB.setLed(HealthBar, Red);
		RGB.setLed(Team, color, B_100);
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
		RGB.setLed(Team, Green, B_100, Blink);
	
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

void testMode()
{
	while(1)
	{
		if(MIRP2.msgReady)
		{
			uint8_t color;
			cli();
			switch(MIRP2.data[2])
			{
				case 0:
					color = Red;
					break;
				case 2:
					color = Blue;
					break;
				case 3:
					color = Green;
					break;
				default:
					color = NoColor;
					break;
			}
			MIRP2.msgReady = false;
			sei();
			RGB.setLed(Team, Green);
			RGB.setLed(HealthBar, color);
			delay(100);
			RGB.setLed(Team, Red);
		}
	}
}