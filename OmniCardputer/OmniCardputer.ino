/*
 Name:		OmniCardputer.ino
 Created:	5/7/2025 6:24:43 PM
 Author:	Deci
*/


// ReSharper disable CppDeclaratorNeverUsed
#include <ArduinoHttpClient.h>
#include <M5Unit-CatM.h>
#include <string>
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
StreamDebugger g_dbg(SerialAT, SerialMon);
TinyGsm        g_modem(g_dbg);
#else
TinyGsm g_modem(SerialAT);
#endif

IPAddress g_host{206, 196, 32, 11} PROGMEM;

TinyGsmClient g_gsmClient(g_modem);
HttpClient    g_http{g_gsmClient, g_host, 22000};


static bool g_smsMode = false;

static const String g_myNumber{"6127876708"} PROGMEM;

StringSumHelper g_strBuf{256};

void setup()
{
	SerialMon.begin(MONITOR_BAUDRATE);
	SerialMon.println("Starting");

	for (auto c = g_strBuf.begin(); c != g_strBuf.end(); c++) {
		*c = '\0';
	}

	delay(300);

	auto cfg            = M5.config();
	cfg.serial_baudrate = MONITOR_BAUDRATE;
	M5Cardputer.begin(cfg, true);

	SerialAT.begin(SIM7080_BAUDRATE, SERIAL_8N1, 1, 2);

	M5Cardputer.Display.init();
	M5Cardputer.Display.clear();

	// M5Cardputer.Display.progressBar(0, 0, 100, 100, 0);

	delay(300);
	g_modem.restart();
	g_modem.init();

	auto info = g_modem.getModemInfo();
	auto imei = g_modem.getIMEI();

	char buf[256];
	sprintf(buf, "%s [%s]\n", info.c_str(), imei.c_str());
	SerialMon.print(buf);
	M5Cardputer.Display.print(buf);

	auto op = g_modem.getOperator();
	SerialMon.println(op);
	M5Cardputer.Display.printf("Operator: %s\n", op.c_str());

	M5Cardputer.Display.print("\nGPRS connecting...");

	auto gp = g_modem.gprsConnect("wholesale");

	M5Cardputer.Display.printf("%d\nNetwork connecting...", gp);

	if (g_modem.waitForNetwork()) {
		SerialMon.println("connected");
		M5Cardputer.Display.printf("connected\n");
		/*M5Cardputer.Display.printf("connecting to client...");
		auto get  = httpClient.get("/");
		auto code = httpClient.responseStatusCode();
		auto len  = httpClient.contentLength();
		M5Cardputer.Display.printf("get: %d | code: %d | len: %lu\n", get, code, len);*/
	} else {
		SerialMon.println("connect fail");
		M5Cardputer.Display.printf("connection failed\n");
	}
}


void loop()
{
	M5Cardputer.update();
	// const char* msg;

	auto state = M5Cardputer.Keyboard.keysState();

	bool isChange = M5Cardputer.Keyboard.isChange();


	if (g_smsMode && isChange) {
		// msg = state.word.data();
		// msg = state.word.data();

		auto word = state.word;

		g_strBuf.concat(word.data());
		auto ss2 = String(g_strBuf);

		M5Cardputer.Display.clear();

		M5Cardputer.Display.drawString(ss2, 0, 0);

		if (state.enter) {
			// M5Cardputer.Display.clear();
			// state.reset();

			M5Cardputer.Display.drawString(ss2, 0, 0);

			g_modem.sendSMS(g_myNumber, ss2);

			g_smsMode = false;
		} else {
			// M5Cardputer.Display.clear();
		}
		SerialMon.printf("%d %d [%s]\n", g_smsMode, ss2.length(), ss2.c_str());
	} else if ((state.fn && M5Cardputer.Keyboard.isKeyPressed('s'))) {
		g_smsMode = true;
		// M5Cardputer.Display.clear();
	} else {
	}
	M5Cardputer.Keyboard.updateKeysState();

	// state.reset();
}
