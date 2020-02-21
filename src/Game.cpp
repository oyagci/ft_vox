#include "Game.hpp"
#include "Time.hpp"
#include "Settings.hpp"
#include "stb_image.h"

Game::Game()
{
	_state.game = MAIN_MENU;

	_display = std::make_unique<Display>("ft_vox", 1280, 720);
	_display->enableCap(GL_DEPTH_TEST);
	_display->enableCap(GL_CULL_FACE);
	_display->enableCap(GL_BLEND);
	_display->setFullscreen(true);

	maths::transform t = { glm::vec3(32, 64, 32), glm::quat(), glm::vec3(1), nullptr };
	_camera = std::make_unique<Camera>(*_display, t);
	_camera->setProjection(glm::radians(80.0f), 0.1f, 1000.0f);

	_world = std::make_unique<World>(*_camera);
	_hud = std::make_unique<PlayerHUD>();
	_fpsCounter = std::make_unique<FPSCounter>();
	_textRenderer = std::make_unique<TextRenderer>();
	_mainMenu = std::make_unique<MainMenu>([this] {
		onStartPlaying();
	});

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0x87 / 255.0f, 0xCE / 255.0f, 0xEB / 255.0f, 1.0f);
}

int Game::run()
{
	Settings::instance().load("config.ini");

	FPSCounter fpsCounter;
	PlayerHUD hud;

	while (!_display->isClosed())
	{
		Time::instance().update();
		_display->update();
		_display->updateInputs();
		_camera->update();
		_fpsCounter->update(Time::getDeltaTime());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (_state.game) {
			case MAIN_MENU:
				mainMenu();
				break ;
			case PLAYING:
				play();
				break ;
			case PAUSED:
				break ;
			default:
				break ;
		}

	}

	return EXIT_SUCCESS;
}

void Game::action(GameAction action)
{
	switch (action) {
		case PLAY:
			_display->showCursor(false);
			_state.game = PLAYING;
			break ;
		case PAUSE:
			_display->showCursor(true);
			_state.game = PAUSED;
			break ;
		default:
			break ;
	};
}

void Game::play()
{
	float deltaTime = Time::getDeltaTime();

	float ms = 30.0f;
	if (input::getKeyboard().getKey(GLFW_KEY_LEFT_SHIFT)) {
		ms = 500.0f;
	}
	if (input::getKeyboard().getKey(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(_display->getWindow(), GLFW_TRUE);
	}

	_camera->input(ms * deltaTime, 0.001f, {
		GLFW_KEY_W,
		GLFW_KEY_S,
		GLFW_KEY_A,
		GLFW_KEY_D,
		GLFW_KEY_LEFT_CONTROL,
		GLFW_KEY_SPACE
	});

	glDisable(GL_BLEND);
	_world->update();
	_world->render();
	glEnable(GL_BLEND);
	_hud->draw();

	_textRenderer->drawText(std::to_string(_fpsCounter->getFPS()) + " FPS", glm::vec2(10, 10), .3f, glm::vec3(1.0f));
}

void Game::mainMenu()
{
	_mainMenu->update();
	_mainMenu->render();
}

void Game::onStartPlaying()
{
	action(PLAY);
}