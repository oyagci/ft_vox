#pragma once
#include <iostream>
#include <functional>
#include "Callback.hpp"

template <typename... Args>
class Action
{
private:
	using ActionCallback = Callback<Args...>;

	std::vector<ActionCallback> _subscribers;

public:
	Action()
	{
	}

	void operator()(Args... args)
	{
		for (auto &s : _subscribers) {
			s(std::forward<Args>(args)...);
		}
	}

	void operator+=(ActionCallback callback)
	{
		_subscribers.push_back(callback);
	}

	void operator-=(ActionCallback callback)
	{
		auto cb = std::find(_subscribers.begin(), _subscribers.end(), callback);
		if (cb != _subscribers.end()) {
			_subscribers.erase(cb);
		}
	}
};

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
