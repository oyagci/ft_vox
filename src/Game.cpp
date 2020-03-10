#include "Game.hpp"
#include "Time.hpp"
#include "Settings.hpp"
#include "stb_image.h"

Game::Game()
{
	_state.game = MAIN_MENU;

	_display = std::make_unique<Display>("ft_vox", 1920, 1080);
	_display->enableCap(GL_DEPTH_TEST);
	_display->enableCap(GL_CULL_FACE);
	_display->enableCap(GL_BLEND);
	_display->setFullscreen(false);

	maths::transform t = { glm::vec3(32, 64, 32), glm::quat(), glm::vec3(1), nullptr };
	_camera = std::make_unique<Camera>(*_display, t);
	_camera->setProjection(glm::radians(80.0f), 0.1f, 1000.0f);

	_world = std::make_unique<World>(*_camera);
	_fpsCounter = std::make_unique<FPSCounter>();
	_textRenderer = std::make_unique<TextRenderer>(_display->getWidth(), _display->getHeight());
	_ui = std::make_unique<UI>(_display->getWidth(), _display->getHeight());

	_ui->registerFunc("playSingleplayer", [this] { onStartPlaying(); });
	_ui->registerFunc("quitGame", [this] { onExit(); });

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0x87 / 255.0f, 0xCE / 255.0f, 0xEB / 255.0f, 1.0f);

	GameEvents::instance().onKeyboardKeypress += Callback<int>([this] (int keycode) {
		_ui->call("setHotbarItem", keycode - GLFW_KEY_1);
	});

	std::vector<std::pair<std::string, int>> keybinds = {
		{ "Jump",   GLFW_KEY_SPACE },
		{ "Fire",   GLFW_KEY_SPACE },
		{ "Sprint", GLFW_KEY_LEFT_SHIFT },
	};
	std::vector<std::tuple<std::string, int, float>> axisBinds = {
		{ "MoveForward", GLFW_KEY_W,  1.0f },
		{ "MoveForward", GLFW_KEY_S, -1.0f },
		{ "MoveRight",   GLFW_KEY_D,  1.0f },
		{ "MoveRight",   GLFW_KEY_A, -1.0f },
	};

	_inputController = std::make_unique<InputController>();
	for (auto const &k : keybinds) {
		_inputController->mapAction(k.first, k.second);
	}
	for (auto const &a : axisBinds) {
		_inputController->mapAxis(std::get<0>(a), std::get<1>(a), std::get<2>(a));
	}

	_inputController->bindAction("Jump", IE_Pressed, Callback<>([] {
		std::cout << "Start Jump" << std::endl;
	}));
	_inputController->bindAction("Jump", IE_Released, Callback<>([] {
		std::cout << "Stop Jump" << std::endl;
	}));
	_inputController->bindAxis("MoveRight", Callback<float>([] (float amount) {
		std::cout << "MoveRight : " << amount << std::endl;
	}));
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

		_ui->update();
		_ui->render();

	}

	return EXIT_SUCCESS;
}

void Game::action(GameAction action)
{
	switch (action) {
		case PLAY:
			_display->showCursor(false);
			_ui->showScene("playerHud");
			_state.game = PLAYING;
			break ;
		case PAUSE:
			_display->showCursor(true);
			_state.game = PAUSED;
			break ;
		case EXIT:
			_state.game = EXITING;
			glfwSetWindowShouldClose(_display->getWindow(), GLFW_TRUE);
		default:
			break ;
	};
}

void Game::play()
{
	float deltaTime = Time::getDeltaTime();

	float ms = 30.0f;
//	if (input::getKeyboard().getKey(GLFW_KEY_LEFT_SHIFT)) {
//		ms = 500.0f;
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_ESCAPE)) {
//		glfwSetWindowShouldClose(_display->getWindow(), GLFW_TRUE);
//	}
//
//	if (input::getKeyboard().getKey(GLFW_KEY_1)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_1);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_2)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_2);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_3)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_3);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_4)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_4);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_5)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_5);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_6)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_6);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_7)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_7);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_8)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_8);
//	}
//	if (input::getKeyboard().getKey(GLFW_KEY_9)) {
//		GameEvents::instance().KeyboardKeypress(GLFW_KEY_9);
//	}

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

//	_textRenderer->drawText(std::to_string(_fpsCounter->getFPS()) + " FPS", 0.5f, glm::vec3(1.0f));
}

void Game::mainMenu()
{
}

void Game::onStartPlaying()
{
	action(PLAY);
}

void Game::onExit()
{
	action(EXIT);
}
