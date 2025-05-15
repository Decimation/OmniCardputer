// 
// 
// 

#include "MenuOption.h"



void MenuOptionClass::displayMenu(std::vector<MenuOptionClass> options)
{
	do {
		M5Cardputer.Display.clearDisplay();
		for (auto e = options.begin(); e != options.end(); ++e) {
			M5Cardputer.Display.print(e->name);
		}

		if (M5Cardputer.Keyboard.isChange()) {
			auto state = M5Cardputer.Keyboard.keysState();
			auto words = state.word;
			SerialMon.printf("[%s] [%d]\n", words.data(), words.size());

			for (const char wc : words) {
				int c     = char_to_int(wc);
				int isDig = isdigit(c) && (c >= 0 && c < options.size());
				options[c].func();

			}
		}
	}
	while (true);
}
