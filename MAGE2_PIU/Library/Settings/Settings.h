/*******************************************************************************
* Name: Settings.h
* Description: SD Card Settings Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/

#ifndef SETTINGS_h
#define SETTINGS_h

#include <Arduino.h>
#include <SD.h>

#define SDPin 10

class SettingsClass
{
	public:
		boolean init();
		boolean read();
		uint16_t player_id;
		boolean debugEnabled;
	private:
		File config;
};

extern SettingsClass Settings;

#endif
