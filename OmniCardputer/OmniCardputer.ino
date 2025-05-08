/*
 Name:		OmniCardputer.ino
 Created:	5/7/2025 6:24:43 PM
 Author:	Deci
*/


// the setup function runs once when you press reset or power the board
#include <M5Unit-CatM.h>

#define TINY_GSM_DEBUG SerialMon
#define DUMP_AT_COMMANDS

#include <TinyGsmClient.h>
#include <M5GFX.h>
#include <M5Unified.h>
// #include <M5Utility.h>
// #include <M5_SIM7080G.h>
#include <M5Cardputer.h>


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client;

void setup()
{
	// M5Cardputer.begin(true);
	SerialMon.begin(MONITOR_BAUDRATE);
	SerialMon.println("Starting");
	auto cfg            = M5.config();
	cfg.serial_baudrate = MONITOR_BAUDRATE;
	M5Cardputer.begin(cfg, true);
	 
	SerialAT.begin(SIM7080_BAUDRATE, SERIAL_8N1, 1, 2);
	modem.init();
	client = TinyGsmClient(modem);
	// SerialMon.println("Starting");


	SerialMon.println("Starting");
	auto info = modem.getModemInfo();
	SerialMon.println(info);
	auto imei = modem.getIMEI();

	char buf[256];
	sprintf(buf, "%s [%s]\n", info.c_str(), imei.c_str());
	SerialMon.println(buf);

	auto op = modem.getOperator();
	SerialMon.println(op);

	// int result = modem.setNetworkMode(38);
	// Serial.printf("Set network MODE: %s\n", result ? "Ok" : "No");

	/*modem.sendAT(GF("+CMNB=3"));                    //  set auto NB-IOT / CAT-M
	if (modem.waitResponse(5000L)) {
		Serial.printf("Auto NB-IOT/CAT-M set\n");
	} else {
		Serial.printf("Auto NB-IOT/CAT-M fail!\n");
	}*/

	auto gp = modem.gprsConnect("wholesale");
	SerialMon.println(gp);

	if (modem.waitForNetwork()) {
		SerialMon.println("connected");
	} else {
		SerialMon.println("connect fail");
		return;
	}

}




// the loop function runs over and over again until power down or reset
void loop()
{
	M5Cardputer.update();
}
