#pragma once

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
