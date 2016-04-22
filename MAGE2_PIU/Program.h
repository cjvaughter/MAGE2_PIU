#ifndef PROGRAM_h
#define PROGRAM_h

#include <Arduino.h>
#include <Bluetooth.h>
#include <Constants.h>
#include <Debugger.h>
#include <Haptic.h>
#include <MIRP2.h>
#include <RGB.h>
#include <Settings.h>
#include <XBee.h>

#ifdef DEBUG
	#define MAX_CONNECT_ATTEMPTS 5
#else
	#define MAX_CONNECT_ATTEMPTS 30
#endif

class ProgramClass : Debugger
{
private:
	uint8_t state;
	uint8_t health;
	uint8_t effect;
	uint64_t currentTime;
	uint16_t player_id;
	uint8_t team;
	uint8_t lastDirection;
	boolean connectionRequest;
	uint64_t nextRequestTime;
	boolean testMode;
	uint8_t connectCounter;
	boolean serverTimeout;
	uint8_t btTestCase;

public:
	ProgramClass() : Debugger()
	{
		state = Dead;
		health = 0;
		effect = 0;
		currentTime = 0;
		player_id = 0;
		team = Red;
		lastDirection = NoDirection;
		connectionRequest = false;
		nextRequestTime = 0;
		testMode = false;
		connectCounter = 0;
		serverTimeout = false;
		btTestCase = 0;
	}
	void setup();
	void loop();
	void changeWeapon();
	void FatalError(const char* message, uint8_t color = Red);
	void reset(boolean DFU = false);
};

//hook onto Arduino control flow
ProgramClass Program;
void setup()
{
	Program.setup();
}
void loop()
{
	Program.loop();
}

#endif
