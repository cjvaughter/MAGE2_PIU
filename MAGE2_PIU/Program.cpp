#include <Arduino.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController controller = ThreadController();
//Thread heartbeatThread = Thread();
//Thread serverThread = Thread();
//Thread bluetoothThread = Thread();
Thread uxThread = Thread();

void ux();
		            
void setup()
{
	DDRB |= 1;
	Serial.begin(38400);
	XBee.begin(&Serial1);
	//Bluetooth.setPort(&Serial);
	//heartbeatThread.onRun(XBeeClass::heartbeat);
	//heartbeatThread.setInterval(2000);
	//serverThread.onRun(XBee.run);
	//bluetoothThread.onRun(&Bluetooth.run);
	//uxThread.onRun(&UX.run);
	uxThread.onRun(ux);
	uxThread.setInterval(1000);
	
	
	//controller.add(&serverThread);
	//controller.add(&bluetoothThread);
	controller.add(&uxThread);
	//controller.add(&heartbeatThread);
}

void loop()
{
	controller.run();
}

void ux()
{
	Serial.println(millis());
	PORTB ^= 1;
	if(PORTB & 1) XBee.heartbeat();
}
