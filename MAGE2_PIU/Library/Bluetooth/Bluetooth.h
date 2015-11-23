/*******************************************************************************
* Name: Bluetooth.h
* Description: RN4677 Bluetooth Library
* Author: Jacob Dixon
*******************************************************************************/

#ifndef BLUETOOTH_h
#define BLUETOOTH_h

#include <Arduino.h>

#define BT Serial3
#define BTDelimiter 0x3C
#define Check 0xFF

enum BluetoothMsg
{
	BTHeartbeat,
	BTConnect,
    BTSpell_TX,
	BTUpdate,
	BTACK = 0xFF
};

class BluetoothClass
{
	public:
	boolean init();
	boolean find_baud();
	boolean enter_at_mode();
	boolean exit_at_mode();
	boolean command(const char* cmd, const char* data);
	boolean command(const char* cmd);
	boolean wait_for_lf();
	void run();
	void update(uint8_t health, uint8_t status, uint8_t effect);
	void ack();
	uint8_t rx_func;
	uint8_t rx_data[3];
	boolean msgReady;
	boolean connected;
	
	private:
	uint8_t _step;
	uint16_t _sum;
	uint8_t _checksum;
};

extern BluetoothClass Bluetooth;

#endif
