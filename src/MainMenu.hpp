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
	MainMenu() = delete;
	MainMenu(MainMenu const &) = delete;
	MainMenu(glm::vec2 size, std::function<void()> onStartPlaying, std::function<void()> onExit);

	void operator=(MainMenu const &) = delete;

	void update();
	void render();

private:
	Mesh _background;
	Shader _shader;
	Shader _buttonShader;
	TextRenderer tr;
	glm::vec2 _size;

	std::unique_ptr<Button> _playButton;
	std::unique_ptr<Button> _exitButton;

	std::function<void()> _onStartPlaying;
	std::function<void()> _onExitGame;
};
