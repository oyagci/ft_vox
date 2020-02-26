#include "MainMenu.hpp"
#include "TextureManager.hpp"
#include "Button.hpp"
#include "Profiler.hpp"

MainMenu::MainMenu(Display &display, glm::vec2 size, std::function<void()> onStartPlaying, std::function<void()> onExitGame) :
	tr(display.getWidth(), display.getHeight()), _onStartPlaying(onStartPlaying), _onExitGame(onExitGame)
{
	TextureManager::instance().createTexture("MenuBackground", "img/terrain_512.png", {
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
	});

	_size = size;

	glm::mat4 projection = glm::ortho(0.0f, size.x, 0.0f, size.y);

	_shader.addVertexShader("shaders/hud.vs.glsl")
		.addFragmentShader("shaders/hud.fs.glsl")
		.link();
	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix", projection);
	_shader.setUniform1i("tex", 0);
	_shader.unbind();

	glm::vec3 bg[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(_size.x, 0.0f, 0.0f),
		glm::vec3(_size.x, _size.y, 0.0f),
		glm::vec3(0.0f, _size.y, 0.0f),
	};
	glm::uvec3 inds[] = {
		glm::uvec3(0, 1, 2),
		glm::uvec3(0, 2, 3),
	};
	glm::vec2 tex[] = {
		glm::vec2(0.0f,            0.0f),
		glm::vec2(_size.x / 32.0f, 0.0f),
		glm::vec2(_size.x / 32.0f, _size.y / 32.0f),
		glm::vec2(0.0f,            _size.y / 32.0f),
	};

	for (auto const &p : bg) {
		_background.addPosition(p);
	}
	for (auto const &i : inds) {
		_background.addTriangle(i);
	}
	for (auto const &t : tex) {
		_background.addUv(t);
	}
	_background.build();

	_playButton = std::make_unique<Button>(display, glm::vec2(_size.x / 2.0f, _size.y / 2.0f),
			glm::vec2(640.0f, 64.0f), _onStartPlaying, Anchor::Center);
	_playButton->setText("Singleplayer");

	_exitButton = std::make_unique<Button>(display, glm::vec2(_size.x / 2.0f, _size.y / 2.0f - (64.0f + 8.0f)),
			glm::vec2(640.0f, 64.0f), _onExitGame, Anchor::Center);
	_exitButton->setText("Quit Game");

	_buttonShader.addVertexShader("shaders/button.vs.glsl")
		.addFragmentShader("shaders/button.fs.glsl")
		.link();
	_buttonShader.bind();
	_buttonShader.setUniform4x4f("projectionMatrix", projection);
	_buttonShader.unbind();
}

void MainMenu::update()
{
	_playButton->update();
	_exitButton->update();
}

void MainMenu::render()
{
	PROFILER_START();

	_shader.bind();
	TextureManager::instance().bind("MenuBackground", GL_TEXTURE0);
	_background.draw();
	_shader.unbind();

	tr.drawText("ft_vox", glm::vec2(_size.x / 2.0f, _size.y - 100.0f) + glm::vec2(5.0f, -5.0f),
		2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Anchor::Top);
	tr.drawText("ft_vox", glm::vec2(_size.x / 2.0f, _size.y - 100.0f),
		2.0f, glm::vec3(1.0f, 1.0f, 1.0f), Anchor::Top);

	_buttonShader.bind();
	_playButton->draw();
	_buttonShader.bind();
	_exitButton->draw();
	_buttonShader.unbind();

	tr.drawText("ft_vox v0.1", glm::vec2(10.0f, 10.0f), 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));

	PROFILER_STOP();
}
