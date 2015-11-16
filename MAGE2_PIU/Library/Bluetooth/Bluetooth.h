/*******************************************************************************
* Name: Bluetooth.h
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef BLUETOOTH_h
#define BLUETOOTH_h

#include <Arduino.h>

#define BT Serial2
#define BTDelimiter 0x3C
#define ACK 0xFF
#define Check 0xFF

class BluetoothClass
{
	public:
	boolean init();
	boolean enter_at_mode();
	boolean exit_at_mode();
	boolean command(const char* cmd, const char* data);
	boolean wait_for_char(char data);
	void run();
	void write();
	uint8_t color;
	uint16_t device_id;
	uint8_t rx_func, tx_func;
	uint8_t device_status;
	boolean msgReady;
	
	private:
	uint8_t _step;
	uint16_t _sum;
	uint8_t _checksum;
	void ack();
};

extern BluetoothClass Bluetooth;

#endif
