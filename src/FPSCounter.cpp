#include "lazy.hpp"
#include "FPSCounter.hpp"

FPSCounter::FPSCounter() : _fps(0), _elapsedTime(0), _lastTime(0), _nFrames(0)
{
}

void FPSCounter::update(float deltaTime)
{
	_nFrames += 1;
	_elapsedTime += deltaTime;
	if (_elapsedTime >= 1.0f) {
		_elapsedTime = 0.0f;
		_fps = _nFrames;
		_nFrames = 0;
	}
}

int FPSCounter::getFPS() const
{
	return _fps;
}
