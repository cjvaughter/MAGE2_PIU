/******************************************************************************
RN4677BT.h
Microchip Bluetooth Dual Mode model RB4677 Library Header File
Jacob Dixon
Original Creation Date: August 30, 2015
Version A
******************************************************************************/

#ifndef RN4677BT_h
#define RN4677BT_h
#include <Arduino.h>

String MACaddr = "";


class RB4677BTClass
{
	public:
		void classicMasterInit(); //master (central) config with BT 2.1 mode aka "Classic"
		void classicSlaveInit(); //slave (peripheral) config with BT 2.1 mode aka "Classic"
		void BLEMasterInit(); //master (central) config with BT 4.0 mode aka "BLE"
		void BLESlaveInit(); //slave (peripheral) config with BT 4.0 mode aka "BLE"
		void scanClassic(MACaddr); //scans for devices for BT Classic Mode
		void scanBLE(MACaddr); // scans for devices using BLE Mode
		void connectClassic(MACaddr); //connects to devices using classic mode
		void connectBLE(MACaddr);
		void setName(String name = "");
		void reboot();
		void reset(); //restore to factory settings
		void disconnect();
		void displayFirmware();
		void displayConfiguration();
};

extern RB4677BTClass bluetooth;
		
#endif