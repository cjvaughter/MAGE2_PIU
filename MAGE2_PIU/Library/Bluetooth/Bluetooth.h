/*******************************************************************************
* Name: Bluetooth.h
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef BLUETOOTH_h
#define BLUETOOTH_h
#include <Arduino.h>
#include <Debugger.h>

#define BTDelimiter 0x3C
#define ACK 0xFF
#define Check 0xFF

class BluetoothClass
{
	public:
	void init();
	void enter_at_mode();
	void exit_at_mode();
	void command(const char* cmd, const char* data);
	void read();
	void write();
	uint8_t color;
	uint16_t device_id;
	uint8_t rx_func, tx_func;
	uint8_t device_status;
	bool msgReady;
	
	private:
	uint8_t _step;
	uint16_t _sum;
	uint8_t _checksum;
	void ack();
};

extern BluetoothClass Bluetooth;

#endif
