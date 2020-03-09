#pragma once

#include <functional>

class BaseCallback
{
public:
	virtual ~BaseCallback() {}
};


template <typename ... Args>
class Callback : public BaseCallback
{
private:
	typedef std::function<void(Args...)> FuncType;
	FuncType _f;

public:
	Callback() {}
	Callback(FuncType f) { _f = f; }
	void operator()(Args... args) { if (_f) { _f(args...); } }
};
