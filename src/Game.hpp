#pragma once

#include <memory>
#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "PlayerHUD.hpp"
#include "FPSCounter.hpp"
#include "World.hpp"
#include "MainMenu.hpp"
#include "UI.hpp"

using namespace lazy;
using namespace graphics;
using namespace inputs;
using namespace utils;

class Game
{
private:
	enum StateVal {
		MAIN_MENU,
		PLAYING,
		PAUSED,
		EXITING,
	};

	enum GameAction {
		PLAY,
		PAUSE,
		EXIT,
	};

	struct GameState {
		StateVal game;
	};
public:
	Game();

	int run();
	void onStartPlaying();
	void onExit();

private:
	void action(GameAction action);
	void play();
	void mainMenu();

	std::unique_ptr<Display> _display;
	std::unique_ptr<TextRenderer> _textRenderer;
	std::unique_ptr<Camera> _camera;
	std::unique_ptr<World> _world;
	std::unique_ptr<PlayerHUD> _hud;
	std::unique_ptr<FPSCounter> _fpsCounter;
	std::unique_ptr<MainMenu> _mainMenu;
	std::unique_ptr<UI> _ui;
	GameState _state;
};
