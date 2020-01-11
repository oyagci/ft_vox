#pragma once

#include "lazy.hpp"

class DebugUI
{
public:
	DebugUI(GLFWwindow *window);
	~DebugUI();

	void update();
	void render();

	void setActive(bool active) { _isActive = active; }
	bool isActive() const { return _isActive; }

private:
	DebugUI() {};

	void setupUI();

	bool _isActive;
};
