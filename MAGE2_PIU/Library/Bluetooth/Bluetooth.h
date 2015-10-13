/*******************************************************************************
* Name: Bluetooth.h
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef BLUETOOTH_h
#define BLUETOOTH_h
#include <Arduino.h>

//String MACaddr = "";

class BluetoothClass
{
	public:
	void classicMasterInit(); //master (central) config with BT 2.1 mode aka "Classic"
	void classicSlaveInit(); //slave (peripheral) config with BT 2.1 mode aka "Classic"
	void classicSlaveInitSecurityMode1(); //for use with legacy devices which require a passkey
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
};

extern BluetoothClass Bluetooth;

#endif
