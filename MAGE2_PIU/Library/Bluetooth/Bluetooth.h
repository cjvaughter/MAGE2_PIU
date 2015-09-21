/******************************************************************************
Bluetooth.h
Microchip Bluetooth Dual Mode model RN4677 Library Header File
Jacob Dixon
Original Creation Date: August 30, 2015
Version A
******************************************************************************/

#ifndef BLUETOOTH_h
#define BLUETOOTH_h
#include "Arduino.h"
#include "elapsedMillis.h"

#define DELAY_MS 500 //delay between sending config commands

class BluetoothClass
{
	public:
		void setPort(HardwareSerial* port);
		void classicMasterInit(); //master (central) config with BT 2.1 mode aka "Classic"
		void classicSlaveInit(); //slave (peripheral) config with BT 2.1 mode aka "Classic"
		void BLEMasterInit(); //master (central) config with BT 4.0 mode aka "BLE"
		void BLESlaveInit(); //slave (peripheral) config with BT 4.0 mode aka "BLE"
		String scanClassic(String MACaddr); //scans for devices for BT Classic Mode
		String scanBLE(String MACaddr); // scans for devices using BLE Mode
		void connectClassic(String MACaddr); //connects to devices using classic mode
		void connectBLE(String MACaddr);
		void setName(String name = "");
		void reboot();
		void reset(); //restore to factory settings
		void disconnect();
		void displayFirmware();
		void displayConfiguration();
		static const unsigned int timeInterval = 10000; //Bluetooth search time
	private:
		String MACaddr;
		HardwareSerial* _port;
		elapsedMillis timeElapsed;
};

extern BluetoothClass Bluetooth;
		
#endif
