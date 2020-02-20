#pragma once

#include <memory>
#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "Button.hpp"

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
	Shader _buttonShader;
	TextRenderer tr;

	std::unique_ptr<Button> _playButton;

	std::function<void()> _onStartPlaying;
};
