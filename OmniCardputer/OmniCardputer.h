// Author: Deci | Project: OmniCardputer | Name: OmniCardputer.h
// Date: 2025/05/14 @ 17:05:11

#pragma once

#define TINY_GSM_DEBUG SerialMon
#define DUMP_AT_COMMANDS


#include <M5Unit-CatM.h>
#include <string>


#include <TinyGsmClient.h>
#include <M5GFX.h>
#include <M5Unified.h>
// #include <M5Utility.h>
// #include <M5_SIM7080G.h>
#include <M5Cardputer.h>
#include <iterator>
#include <functional>
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
class MenuOptionClass;
StreamDebugger g_dbg(SerialAT, SerialMon);
TinyGsm        g_modem(g_dbg);
#else
TinyGsm g_modem(SerialAT);
#endif

#include <esp_task.h>
#include "esp_task_wdt.h"
#include <esp_task.h>

/*struct keyStroke
{ // DO NOT CHANGE IT!!!!!
	bool                 pressed   = false;
	bool                 exit_key  = false;
	bool                 fn        = false;
	bool                 del       = false;
	bool                 enter     = false;
	bool                 alt       = false;
	bool                 ctrl      = false;
	bool                 gui       = false;
	uint8_t              modifiers = 0;
	std::vector<char>    word;
	std::vector<uint8_t> hid_keys;
	std::vector<uint8_t> modifier_keys;

	// Clear function
	void Clear()
	{
		pressed   = false;
		exit_key  = false;
		fn        = false;
		del       = false;
		enter     = false;
		bool alt  = false;
		bool ctrl = false;
		bool gui  = false;
		modifiers = 0;
		word.clear();
		hid_keys.clear();
		modifier_keys.clear();
	}
};

struct TouchPoint
{
	bool     pressed = false;
	uint16_t x;
	uint16_t y;

	// clear touch to better handle tasks
	void Clear(void)
	{
		pressed = false;
		x       = 0;
		y       = 0;
	}
};*/

int char_to_int(const char c);







enum class Option
{
	NONE = 0,
	SMS  = 1,
	Num  = 2,
};

/*extern volatile bool NextPress     = false;
extern volatile bool PrevPress     = false;
extern volatile bool UpPress       = false;
extern volatile bool DownPress     = false;
extern volatile bool SelPress      = false;
extern volatile bool EscPress      = false;
extern volatile bool AnyKeyPress   = false;
extern volatile bool NextPagePress = false;
extern volatile bool PrevPagePress = false;
extern volatile bool LongPress     = false;*/

/*extern TouchPoint touchPoint;

extern keyStroke KeyStroke;*/


extern unsigned long previousMillis;

extern TaskHandle_t xHandle;


extern IPAddress g_host;

extern TinyGsmClient g_gsmClient;

// HttpClient    g_http{g_gsmClient, g_host, 22000};


extern Option g_optMode;

extern String g_myNumber;

extern String g_strBuf;
