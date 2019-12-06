#pragma once

class FPSCounter
{
public:
	FPSCounter();
	void update(float deltaTime);

	int getFPS() const;

private:
	int _fps;
	float _elapsedTime;
	float _lastTime;
	int _nFrames;
};
