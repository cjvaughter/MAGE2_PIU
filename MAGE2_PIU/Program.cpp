#include <Arduino.h>
#include <Bluetooth.h>
#include <XBee.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController controller = ThreadController();
Thread serverThread = Thread();
Thread heartbeatThread = Thread();
Thread bluetoothThread = Thread();
Thread uxThread = Thread();

void ux();
		            
void setup()
{
	XBee.init(&Serial1);
	//Bluetooth.init(&SerialX);
	//Ux.init();
	//MIRP.init();
	DDRB |= 67;

	serverThread.onRun(XBee.run);
	serverThread.setInterval(1);

	heartbeatThread.onRun(XBee.heartbeat);
	heartbeatThread.setInterval(2000);
	
	//bluetoothThread.onRun(blah blah blah);
	//bluetoothThread.setInterval(1);
	
	//uxThread.onRun(ux);
	//uxThread.setInterval(1000);
	
	controller.add(&serverThread);
	controller.add(&heartbeatThread);
	//controller.add(&bluetoothThread);
	//controller.add(&uxThread);
	//MIRP.start();
	XBee.tx_data[0] = 1;
	XBee.tx_data[1] = 0xCC;
	XBee.tx_data[2] = 0xCC;
	XBee.tx_length = 3;
}

void loop()
{
	controller.run();
	if(XBee.msgReady)
	{
		if(XBee.rx_data[0] == 1)
		{
			switch(XBee.rx_data[1])
			{
				case 0:
					PORTB = 64;
					break;
				case 1:
					PORTB = 66;
					break;
				case 2:
					PORTB = 66;
					break;
				case 3:
					PORTB = 2;
					break;
				case 4:
					PORTB = 1;
					break;
				case 5:
					PORTB = 65;
					break;
			}
		}
		XBee.msgReady = false;
	}
}
