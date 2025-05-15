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

TaskHandle_t xHandle;

// Public Globals Variables
unsigned long previousMillis = millis();

IPAddress g_host{206, 196, 32, 11};

TinyGsmClient g_gsmClient(g_modem);

// HttpClient    g_http{g_gsmClient, g_host, 22000};


Option g_optMode = Option::NONE;

String g_myNumber{"6127876708"};

String g_strBuf{};

/*void InputHandler(void)
{
	static unsigned long tm = 0;
	if (millis() - tm < 200 && !LongPress) return;

	bool shoulder = digitalRead(0);
	M5Cardputer.Keyboard.update();
	if (M5Cardputer.Keyboard.isPressed() || shoulder == LOW) {
		tm = millis();
		// if (!wakeUpScreen()) AnyKeyPress = true;
		else return;
		keyStroke                 key;
		Keyboard_Class::KeysState status   = Keyboard.keysState();
		bool                      arrow_up = false;
		bool                      arrow_dw = false;
		bool                      arrow_ry = false;
		bool                      arrow_le = false;
		if (shoulder == LOW) SelPress = true;
		for (auto i : status.hid_keys) key.hid_keys.emplace_back(i);
		for (auto i : status.word) {
			if (i == '`' || i == KEY_BACKSPACE) EscPress = true;

			if (i == ';') {
				arrow_up  = true;
				PrevPress = true;
			}
			if (i == '.') {
				arrow_dw  = true;
				NextPress = true;
			}
			if (i == '/') {
				arrow_ry      = true;
				NextPress     = true;
				NextPagePress = true;
			}
			if (i == ',') {
				arrow_le      = true;
				PrevPress     = true;
				PrevPagePress = true;
			}
			if (status.fn && arrow_up) key.word.emplace_back(0xDA);
			else if (status.fn && arrow_dw) key.word.emplace_back(0xD9);
			else if (status.fn && arrow_ry) key.word.emplace_back(0xD7);
			else if (status.fn && arrow_le) key.word.emplace_back(0xD8);
			else if (status.fn && i == '`') key.word.emplace_back(0xB1);
			else key.word.emplace_back(i);
		}
		// Add CTRL, ALT and Tab to keytroke without modifier
		key.alt  = status.alt;
		key.ctrl = status.ctrl;
		key.gui  = status.opt;
		// Add Tab key
		if (status.tab) key.word.emplace_back(0xB3);

		for (auto i : status.modifier_keys) key.modifier_keys.emplace_back(i);
	skip_mod:
		if (status.del) key.del = true;
		if (status.enter) {
			key.enter    = true;
			key.exit_key = true;
			SelPress     = true;
		}
		if (status.fn) key.fn = true;
		if (key.fn && key.del) {
			key.word.emplace_back(0xD4);
			key.del = false;
			key.fn  = false;
		}
		key.pressed = true;
		KeyStroke   = key;
	} else KeyStroke.Clear();
}


void __attribute__((weak)) taskInputHandler(void* parameter)
{
	auto timer = millis();
	while (true) {
		// Sometimes this task run 2 or more times before looptask,
		// and navigation gets stuck, the idea here is run the input detection
		// if AnyKeyPress is false, or rerun if it was not renewed within 75ms (arbitrary)
		// because AnyKeyPress will be true if didn´t passed through a check(bool var)
		if (!AnyKeyPress || millis() - timer > 75) {
			NextPress          = false;
			PrevPress          = false;
			UpPress            = false;
			DownPress          = false;
			SelPress           = false;
			EscPress           = false;
			AnyKeyPress        = false;
			NextPagePress      = false;
			PrevPagePress      = false;
			touchPoint.pressed = false;
			touchPoint.Clear();
			InputHandler();
			timer = millis();
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}*/


/*template <typename Iterator>
static void clear(Iterator sz, char v = '\0')
{
	for (auto c = sz.begin(); c != sz.end(); c++) {
		*c = v;
	}
}*/

void clear(String& sz, char v)
{
	for (int i = 0; i < sz.length(); i++) {
		// sz[i] = v;
		sz.setCharAt(i, v);
	}
}

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

	/*switch (g_optMode) {
		case Option::SMS:
			{
				if (!isChange) {
					break;
				}

				// msg = state.word.data();
				// msg = state.word.data();


				if (state.del) {
					unsigned li = g_strBuf.length() - 1;
					if (li >= 0) {
						g_strBuf.remove(li);
					}
				} else if (state.enter) {
					// M5Cardputer.Display.clear();
					// state.reset();

					// M5Cardputer.Display.println(g_strBuf);
					M5Cardputer.Display.setTextColor(lgfx::colors::TFT_GREEN);

					g_modem.sendSMS(g_myNumber, g_strBuf);
					g_strBuf.clear();

					g_optMode = Option::NONE;

					// return;
				} else {
					// M5Cardputer.Display.clear();

					auto word = state.word;

					if (word.size() < 1) {
						return;
					}

					for (uint8_t i = 0; i < word.size(); i++) {
						g_strBuf.concat(word[i]);
					}

					SerialMon.printf("[%p] [%s][%d]\n", word.data(), word.data(), word.size());
				}


				// std::copy(word.begin(), word.end(), g_strBuf);
				displayMessage();
				break;
			}

		default:
			{
				if (state.fn) {
					if (M5Cardputer.Keyboard.isKeyPressed('s')) {
						g_optMode = Option::SMS;
						M5Cardputer.Display.setTextColor(lgfx::colors::TFT_BLUE);
					}
				}
			}
			break;
	}*/

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
