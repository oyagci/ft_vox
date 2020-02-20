#pragma once

#include "lazy.hpp"
#include "TextRenderer.hpp"

using lazy::graphics::Mesh;
using lazy::graphics::Shader;

class MainMenu
{
public:
	MainMenu(std::function<void()> onStartPlaying);

	void update();
	void render();

private:
	Mesh _background;
	Shader _shader;
	TextRenderer tr;

	std::function<void()> _onStartPlaying;
};
