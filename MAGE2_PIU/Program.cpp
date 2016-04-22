#include "Program.h"

void ProgramClass::setup()
{	
	Serial.begin(38400);
	MAINLOG("Booting up...");
	if(Settings.init()) 
	{
		if(Settings.read())
		{
			player_id = Settings.player_id;
			if(player_id == 0)
				FatalError("Invalid player ID!");
			if(Settings.coordinatorAddress == 0)
				FatalError("Invalid coordinator address!");
		}
		else
		{
			FatalError("Could not read settings!");
		}
	}
	else
	{
		#ifdef DEBUG
		testMode = true;
		player_id = 0xABCD;
		//player_id = 0x1234;
		Settings.coordinatorAddress = 0x0013A200409377C3;
		#else
		FatalError("No SD card!");
		#endif
	}
	MAINLOG_F("Mode: %s", (testMode) ? "TESTMODE" : "GAMEPLAY");
	MAINLOG_F("Player ID: 0x%04X", player_id);
	MAINLOG_F("Coordinator: 0x%08lX%08lX", (uint32_t)(Settings.coordinatorAddress>>32), (uint32_t)Settings.coordinatorAddress);

	RGB.init();
	XBee.init(Settings.coordinatorAddress);
	Haptic.init();
	MIRP2.init();
	if(!Bluetooth.init())
		#ifdef DEBUG
		LOG("Could not configure Bluetooth module!");
		#else
		FatalError("Could not configure Bluetooth module!", Blue);
		#endif
	
	delay(250);
	MAINLOG("Boot success!");
}

void ProgramClass::loop()
{
	currentTime = millis();

	if(connectionRequest)
	{
		if(currentTime >= nextRequestTime)
		{
			if(connectCounter >= MAX_CONNECT_ATTEMPTS)
			{
				LOG("XBEE", "Server connection timed out");
				connectionRequest = false;
				serverTimeout = true;
			}
			else
			{
				XBee.connect(player_id, Bluetooth.device_id);
				nextRequestTime = currentTime + 1000;
				connectCounter++;
			}
		}
	}
	
	#ifdef BT_TEST
	if(serverTimeout && currentTime >= nextRequestTime)
	{
		switch(btTestCase)
		{
			case 0:
				Bluetooth.update(100, Alive, NoColor);
				btTestCase++;
				break;
			case 1:
				Bluetooth.update(80, Stunned, Blue);
				btTestCase++;
				break;
			case 2:
				Bluetooth.update(0, Dead, Red);
				btTestCase = 0;
				break;
		}
		nextRequestTime = currentTime + 5000;
	}
	#endif

	//XBee transactions
	XBee.run(currentTime);
	if(XBee.available())
	{
		switch(XBee.nextByte())
		{
			case Connect:
				LOG("XBEE", "Connection confirmed");
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
				LOG("XBEE", "Server requested disconnect");
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
				LOG("BLUETOOTH", "Heartbeat");
				Bluetooth.nextHeartbeat = currentTime + 9000;
				Bluetooth.ack();
				break;
			case BTConnect:
				LOG_F_AS("BLUETOOTH", "Connect request - Weapon ID: 0x%02X%02X Color: 0x%02X", Bluetooth.rx_data[0], Bluetooth.rx_data[1], Bluetooth.rx_data[2]);
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
					connectCounter = 0;
					RGB.setLed(HealthBar, Bluetooth.rx_data[2]);
					delay(80);
					RGB.setLed(HealthBar, NoColor);
					delay(80);
					RGB.setLed(HealthBar, Bluetooth.rx_data[2]);
					delay(80);
					RGB.setLed(HealthBar, NoColor);
				}
				Bluetooth.nextHeartbeat = currentTime + 9000;
				break;
			case BTSpell_TX:
				LOG_F_AS("BLUETOOTH", "Spell TX - Unique Field: %#X", Bluetooth.rx_data[0]);
				XBee.tx_data[0] = Spell_TX;
				XBee.tx_data[1] = Bluetooth.rx_data[0];
				Bluetooth.ack();
				XBee.Encode(2);
				Bluetooth.nextHeartbeat = currentTime + 9000;
				break;
			case BTUpdate:
				//Do nothing for now
				//(destructible devices will be in a future update)
				LOG("BLUETOOTH", "Update messages are not yet supported!");
				Bluetooth.ack();
				Bluetooth.nextHeartbeat = currentTime + 9000;
				break;
			case BTACK:
				//do nothing
				break;
			default:
				LOG_F_AS("BLUETOOTH", "Invalid message!  Func: 0x%02X  Data: 0x%02X 0x%02X 0x%02X", Bluetooth.rx_func, Bluetooth.rx_data[0], Bluetooth.rx_data[1], Bluetooth.rx_data[2]);
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
			LOG("BLUETOOTH", "Lost connection - Heartbeat timed out");
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

		#ifdef DEBUG
		const char* directionName;
		switch(lastDirection)
		{
			case Front: directionName = "FRONT"; break;
			case Back: directionName = "BACK"; break;
			case Left: directionName = "LEFT"; break;
			case Right: directionName = "RIGHT"; break;
			default: directionName = ""; break;
		}
		LOG_F("MIRP packet received from %s - 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", directionName, MIRP2.data[0], MIRP2.data[1], MIRP2.data[2], MIRP2.data[3], MIRP2.data[4], MIRP2.data[5]);
		#endif
	}
	
	//UX
	Haptic.run(currentTime);
	RGB.run(currentTime);
}

void ProgramClass::changeWeapon()
{
	XBee.tx_data[0] = ChangeWeapon;
	XBee.tx_data[1] = (byte)(Bluetooth.device_id >> 8);
	XBee.tx_data[2] = (byte)(Bluetooth.device_id);
	XBee.Encode(3);
}

void ProgramClass::FatalError(const char* message, uint8_t color)
{
	MAINLOG_F("FATAL ERROR -> %s", message);

	RGB.init();
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

void ProgramClass::reset(boolean DFU)
{
	RGB.setBlinkRate();
	RGB.setLed(All, NoColor);
	RGB.setLed(Power, Red, B_100, Blink);
	if(DFU)
	{
		LOG("Entering DFU mode...");
		RGB.setLed(Team, Green, B_100, Blink);
	}
	else
	{
		LOG("Resetting...");
	}	
	
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
