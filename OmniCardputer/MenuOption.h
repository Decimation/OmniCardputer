// MenuOption.h

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <vector>
#include <string>
#include <functional>
#include "Util.h"
#include "OmniCardputer.h"

class MenuOptionClass
{
protected:


public:
	String name;

	std::function<void()> func;

	static void displayMenu(std::vector<MenuOptionClass> options);
};
