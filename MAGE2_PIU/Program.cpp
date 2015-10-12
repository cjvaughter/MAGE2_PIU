#include <Arduino.h>
#include <avr/power.h>
#include <Bluetooth.h>
#include <XBee.h>

#ifdef DEBUG
	#define DEBUGGER_BEGIN(x) Serial.begin(x)
	#define DEBUGGER(x) Serial.println(x); Serial.flush()
#else
	#define DEBUGGER_BEGIN(x)
	#define DEBUGGER(x)
#endif

#define SW_VER_MAJ 0
#define SW_VER_MIN 1

enum States
{
	Alive,
	Dead,
	Stunned,
	
	DFU = 0xFF
};

byte state = Dead;
byte connected = 0;
uint64_t currentTime = 0;
uint64_t lastTime = 0;        
			    
void setup()
{
	sei();
	ADCSRA = 0;
	power_adc_disable();
	DEBUGGER_BEGIN(38400);
	DEBUGGER("Program started");
	XBee.init(&Serial1);
	//Bluetooth.init(&SerialX);
	//Ux.init();
	//MIRP.init();
	pinMode(A15, OUTPUT);
	XBee.connect(0xCCCC, 0xCCCC);
}

void loop()
{	
	currentTime = millis();
	
	//XBee transactions 
	if(connected)
	{
		if(currentTime - lastTime >= 2000)
		{
			XBee.heartbeat();
			lastTime = currentTime;
		}
	}
	
	XBee.read();
	if(XBee.msgReady)
	{
		switch(XBee.rx_data[0])
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
		XBee.msgReady = false;
	}
	
	//Bluetooth transactions
	//...
	//...
}