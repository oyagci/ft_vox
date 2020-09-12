#pragma once

#include <memory>
#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "PlayerHUD.hpp"
#include "FPSCounter.hpp"
#include "World.hpp"
#include "ui/UI.hpp"
#include "BlockTargeting.hpp"

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

	static Game &get();

	inline auto &getWorld() { return *_world; }

private:

	inline static Game *_instance = nullptr;

	void action(GameAction action);
	void play();
	void mainMenu();

	std::unique_ptr<Display> _display;
	std::unique_ptr<TextRenderer> _textRenderer;
	std::unique_ptr<Camera> _camera;
	std::unique_ptr<World> _world;
	std::unique_ptr<FPSCounter> _fpsCounter;
	std::unique_ptr<UI> _ui;
	std::unique_ptr<BlockTargeting> _blockTargeting;
	GameState _state;
};
