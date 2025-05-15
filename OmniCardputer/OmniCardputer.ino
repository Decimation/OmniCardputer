/*
 Name:		OmniCardputer.ino
 Created:	5/7/2025 6:24:43 PM
 Author:	Deci
*/


// ReSharper disable CppDeclaratorNeverUsed
// #include <ArduinoHttpClient.h>


// ReSharper disable CppClangTidyCppcoreguidelinesInterfacesGlobalInit


#include "OmniCardputer.h"

// Navigation Variables
/*volatile bool NextPress     = false;
volatile bool PrevPress     = false;
volatile bool UpPress       = false;
volatile bool DownPress     = false;
volatile bool SelPress      = false;
volatile bool EscPress      = false;
volatile bool AnyKeyPress   = false;
volatile bool NextPagePress = false;
volatile bool PrevPagePress = false;
volatile bool LongPress     = false;*/

/*TouchPoint touchPoint;

keyStroke KeyStroke;*/




/*template <typename Iterator>
static void clear(Iterator sz, char v = '\0')
{
	for (auto c = sz.begin(); c != sz.end(); c++) {
		*c = v;
	}
}*/

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger g_dbg(SerialAT, SerialMon);
TinyGsm        g_modem(g_dbg);
#else
TinyGsm g_modem(SerialAT);
#endif


TaskHandle_t xHandle;

// Public Globals Variables
unsigned long previousMillis = millis();

IPAddress g_host{206, 196, 32, 11};

TinyGsmClient g_gsmClient(g_modem);

// HttpClient    g_http{g_gsmClient, g_host, 22000};

Option g_optMode = Option::NONE;

String g_myNumber{"6127876708"};

String g_strBuf{};

void setup()
{
	M5.begin();
	M5.Power.begin();
	g_strBuf.reserve(512);

	SerialMon.begin(MONITOR_BAUDRATE);
	SerialMon.println("Starting");

	clear(g_strBuf, '\0');
	g_strBuf.clear();

	// setup_task();

	delay(300);

	auto cfg            = M5.config();
	cfg.serial_baudrate = MONITOR_BAUDRATE;

	M5Cardputer.begin(cfg, true);

	SerialAT.begin(SIM7080_BAUDRATE, SERIAL_8N1, 1, 2);

	M5Cardputer.Display.init();
	M5Cardputer.Display.clear();
	M5.Display.setCursor(0, 0, 2);


	// M5Cardputer.Display.progressBar(0, 0, 100, 100, 0);


	// g_modem.restart();
	g_modem.init();

	delay(300);

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

static void displayStringInput(String& sz)
{
	SerialMon.printf("[%s] [%d]\n", sz.c_str(), sz.length());

	M5Cardputer.Display.clearDisplay();
	M5.Display.setCursor(0, 0, 2);
	M5Cardputer.Display.printf(">%s\n", sz.c_str());
	// M5Cardputer.Display.println(g_strBuf);
}

static bool readInput(String& sz)
{
	// const char* msg;
	// buf = {};

	// bool display  = true;
	bool ret = true;
	M5Cardputer.Display.setTextColor(lgfx::colors::TFT_BLUE);

	do {
		M5Cardputer.update();

		auto state    = M5Cardputer.Keyboard.keysState();
		bool isChange = M5Cardputer.Keyboard.isChange();

		if (!isChange) {
			continue;
		}
		if (state.fn && M5Cardputer.Keyboard.isKeyPressed('q')) {
			ret = false;
			break;
		}

		if (state.del) {
			unsigned li = sz.length() - 1;
			if (li >= 0) {
				sz.remove(li);
			}
		} else if (state.enter) {
			M5Cardputer.Display.setTextColor(lgfx::colors::TFT_GREEN);
			displayStringInput(sz);

			// g_strBuf.clear();

			// g_optMode = Option::NONE;

			// return;

			ret = true;
			break;
		} else {
			// M5Cardputer.Display.clear();

			auto word = state.word;

			if (word.empty()) {
				continue;
			}

			for (uint8_t i = 0; i < word.size(); i++) {
				sz.concat(word[i]);
			}

			SerialMon.printf("[%p] [%s][%d]\n", word.data(), word.data(), word.size());
		}


		// std::copy(word.begin(), word.end(), g_strBuf);
		M5Cardputer.Keyboard.updateKeysState();
		displayStringInput(sz);

		delay(100);

		// break;
	}
	while (true);

	return ret;
	// state.reset();
}


void loop()
{
	M5Cardputer.update();
	// const char* msg;
	// buf = {};

	auto state    = M5Cardputer.Keyboard.keysState();
	bool isChange = M5Cardputer.Keyboard.isChange();
	// bool display  = true;

	if (state.fn) {
		if (M5Cardputer.Keyboard.isKeyPressed('s')) {
			g_optMode = Option::SMS;


			auto b = readInput(g_strBuf);
			if (b) {
				g_modem.sendSMS(g_myNumber, g_strBuf);
				g_strBuf.clear();
				M5Cardputer.Display.clearDisplay();
			}
		} else if (M5Cardputer.Keyboard.isKeyPressed('n')) {
			g_optMode = Option::SMS;

			// g_myNumber.clear();

			auto b = readInput(g_myNumber);
			if (b) {
				M5Cardputer.Display.printf("Set number to %s\n", g_myNumber.c_str());
				delay(1000);
				M5Cardputer.Display.clearDisplay();
			}
		}
	}


	M5Cardputer.Keyboard.updateKeysState();

	// state.reset();
}
