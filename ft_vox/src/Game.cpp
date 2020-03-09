#include "Game.hpp"
#include "Time.hpp"
#include "Settings.hpp"
#include "stb_image.h"

Game &Game::get()
{
	return *_instance;
}

Game::Game()
{
	_instance = this;
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

	_ui->registerFunc("playSingleplayer", Callback<>([this] { onStartPlaying(); }));
	_ui->registerFunc("quitGame", Callback<>([this] { onExit(); }));

	_gBuffer = std::make_unique<GBuffer>();
	_gBuffer->shader().bind();
	_gBuffer->shader().setUniform1f("fogStart", 128.0f);
	_gBuffer->shader().unbind();

	_finalDrawShader = std::make_unique<lazy::graphics::Shader>();
	_finalDrawShader->addVertexShader("shaders/final.vs.glsl")
		.addFragmentShader("shaders/final.fs.glsl")
		.link();
	_finalDrawShader->bind();
		_finalDrawShader->setUniform1i("colorTexture", 0);
		_finalDrawShader->setUniform1i("positionTexture", 1);
		_finalDrawShader->setUniform1i("normalTexture", 2);
		_finalDrawShader->setUniform1i("depthTexture", 3);
		_finalDrawShader->setUniform1i("ssaoTexture", 4);
	_finalDrawShader->unbind();

	_renderQuad = std::make_unique<Quad>();

	_ssao = std::make_unique<AmbientOcclusion>();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0x87 / 255.0f, 0xCE / 255.0f, 0xEB / 255.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
	if (input::getKeyboard().getKey(GLFW_KEY_LEFT_SHIFT)) {
		ms = 500.0f;
	}
	if (input::getKeyboard().getKey(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(_display->getWindow(), GLFW_TRUE);
	}

	if (input::getKeyboard().getKey(GLFW_KEY_1)) {
		_ui->call("setHotbarItem", 0);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_2)) {
		_ui->call("setHotbarItem", 1);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_3)) {
		_ui->call("setHotbarItem", 2);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_4)) {
		_ui->call("setHotbarItem", 3);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_5)) {
		_ui->call("setHotbarItem", 4);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_6)) {
		_ui->call("setHotbarItem", 5);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_7)) {
		_ui->call("setHotbarItem", 6);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_8)) {
		_ui->call("setHotbarItem", 7);
	}
	if (input::getKeyboard().getKey(GLFW_KEY_9)) {
		_ui->call("setHotbarItem", 8);
	}

	_camera->input(ms * deltaTime, 0.001f, {
		GLFW_KEY_W,
		GLFW_KEY_S,
		GLFW_KEY_A,
		GLFW_KEY_D,
		GLFW_KEY_LEFT_CONTROL,
		GLFW_KEY_SPACE
	});

	if (lazy::inputs::input::getMouse().getButtonDown(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		auto block = BlockTargeting::get().getCurrentTarget();

		if (block) {
			_world->setBlock(block->Position, 0);
		}
	}

	if (lazy::inputs::input::getMouse().getButtonDown(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		auto block = BlockTargeting::get().getCurrentTarget();

		if (block) {
			_world->setBlock(block->Position + glm::ivec3(block->Normal), 2);
		}
	}

	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// G-buffer pass
	{
		_gBuffer->shader().bind();
		_gBuffer->shader().setUniform4x4f("projectionMatrix", _camera->getProjectionMatrix());
		_gBuffer->shader().setUniform4x4f("viewMatrix", _camera->getViewMatrix());
		_gBuffer->shader().setUniform4x4f("viewProjectionMatrix", _camera->getViewProjectionMatrix());
		_gBuffer->shader().setUniform4x4f("modelMatrix", glm::mat4(1.0f));

		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer->framebuffer());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			glDisable(GL_BLEND);
			_world->update();
			_world->render(_gBuffer->shader());
			glEnable(GL_BLEND);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		_gBuffer->shader().unbind();
	}

	_ssao->render(_camera->getProjectionMatrix(), _gBuffer->positionTex(), _gBuffer->normalTex());

	// Post-Process and final draw
	{
		glDisable(GL_DEPTH_TEST);

		_finalDrawShader->bind();
		_finalDrawShader->setUniform4x4f("projectionMatrix", _camera->getProjectionMatrix());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _gBuffer->colorTex());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _gBuffer->positionTex());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _gBuffer->normalTex());

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _gBuffer->depthTex());

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, _ssao->ssaoTexture());

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		_renderQuad->draw();
	}

	glDisable(GL_CULL_FACE);
	BlockTargeting::get().update(_camera->getPosition(), _camera->getTransform().forward());
	BlockTargeting::get().draw();
	glEnable(GL_CULL_FACE);
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
