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
uint16_t player_id = 0;     

File configFile;
File outputFile;
			    
void setup()
{
	sei();
	
	//load sd settings
	
	Debugger.init();
	//XBee.init();
	//Bluetooth.init();
	Haptic.init();
	//RGB.init();
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
	//RGB stuff	
}

/*
	if(userBeginningEntry == "sd")
	{
		Serial.println("==========Begin SD card test.==========");
		delay(500);
		Serial.println("Initializing SD Card...");
		if(!SD.begin(53)) //53 = chip select pin
		{
			Serial.println("Initialization of SD card failed! Please check SD card and restart device.");
			while(1==1);
		}
		Serial.println("Initialization done.");
		
		configFile = SD.open("PIUconfg.txt", FILE_READ);
		if(configFile)
		{
			Serial.println("Configuration file opened.\nReading in Configurations...\n");
			while(configFile.available() != 0) //now print all info in file to console
			{
				Serial.write(configFile.read());
			}
			configFile.close();
		}
		else //file could not open
		{
			Serial.println("Error opening PIU configuration file 'PIUconfg.txt'. Please make sure the file exists and is in the root directory, and restart device.");
			while(1==1);
		}
		
		//open command will create file if it does not already exist
		outputFile = SD.open("PIUout.txt", FILE_WRITE);
		if(outputFile)
		{
			Serial.println("\nWrite something to the PIU output file.");
			Serial.flush();
			while(!Serial.available());
			SDUserInput = Serial.readStringUntil('\n');
			outputFile.println(SDUserInput);
			outputFile.close();
			Serial.println("Written to SD Card.");
			Serial.println("Now printing contents of the PIU output file.");
			outputFile = SD.open("PIUout.txt", FILE_READ);
			while(outputFile.available() != 0) //now print all info in file to console
			{
				Serial.write(outputFile.read());
			}
			outputFile.close();
		}
	}
*/
