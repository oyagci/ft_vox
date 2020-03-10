#pragma once
#include <iostream>
#include <functional>
#include "Action.hpp"

class GameEvents
{
private:
	GameEvents()
	{
	}

public:
	GameEvents(GameEvents &) = delete;
	void operator=(GameEvents &rhs) = delete;

	static GameEvents &instance() {
		static GameEvents current;
		return current;
	}

	Action<int> onKeyboardKeypress;
	void KeyboardKeypress(int keycode)
	{
		onKeyboardKeypress(keycode);
	}
};
