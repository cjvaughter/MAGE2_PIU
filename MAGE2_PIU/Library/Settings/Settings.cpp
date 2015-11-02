/*******************************************************************************
* Name: Settings.cpp
* Description: SD Card Settings Library
* Author: CJ Vaughter & Jacob Dixon
*******************************************************************************/

#include "Settings.h"

SettingsClass Settings;

boolean SettingsClass::init()
{
	player_id = 0;
	debugEnabled = false;
	
	return SD.begin(SDPin);
}

boolean SettingsClass::read()
{
	char* bfr = new char[128];
	uint8_t index = 0;
	uint8_t seperator = 0;
	
	config = SD.open("PIU.ini", FILE_READ);
	if(config)
	{
		while(config.available())
		{
			index = 0;
			seperator = 0;
			if(config.read() != '[') continue;
			while(config.available() && index < 127)
			{
				char c = config.read();
				if(c == ']') break;
				if(c == '=')
				{
					seperator = index;
					c = '\0';
				}
				bfr[index++] = c;
			}
			bfr[index] = '\0';
			
			char* name = &bfr[0];
			char* value = &bfr[seperator+1];
			
			if(strcmp(name, "id") == 0)
			{
				player_id = (uint16_t)strtoul(value, NULL, 16);
			}	
			else if(strcmp(name, "debug") == 0)
			{
				if(strcmp(value, "enabled") == 0)
				{
					debugEnabled = true;
				}
			}
		}
		config.close();
		delete[] bfr;
	}
	else
	{
		delete[] bfr;
		return false;
	}
	return true;
}
