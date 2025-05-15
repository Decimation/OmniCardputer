// MenuOption.h

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MenuOptionClass
{
protected:


public:
	String name;

	std::function<void()> func;
};

extern MenuOptionClass MenuOption;
