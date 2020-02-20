#include "MainMenu.hpp"
#include "TextureManager.hpp"

MainMenu::MainMenu(std::function<void()> onStartPlaying) : _onStartPlaying(onStartPlaying)
{
	TextureManager::instance().createTexture("MenuBackground", "img/terrain_512.png", {
		{ GL_TEXTURE_WRAP_S, GL_REPEAT },
		{ GL_TEXTURE_WRAP_T, GL_REPEAT },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
	});

	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	_shader.addVertexShader("shaders/hud.vs.glsl")
		.addFragmentShader("shaders/hud.fs.glsl")
		.link();
	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix", projection);
	_shader.setUniform1i("tex", 0);
	_shader.unbind();

	glm::vec3 bg[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1280.0f, 0.0f, 0.0f),
		glm::vec3(1280.0f, 720.0f, 0.0f),
		glm::vec3(0.0f, 720.0f, 0.0f),
	};
	glm::uvec3 inds[] = {
		glm::uvec3(0, 1, 2),
		glm::uvec3(0, 2, 3),
	};
	glm::vec2 tex[] = {
		glm::vec2(0.0f,            0.0f),
		glm::vec2(1280.0f / 32.0f, 0.0f),
		glm::vec2(1280.0f / 32.0f, 720.0f / 32.0f),
		glm::vec2(0.0f,            720.0f / 32.0f),
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
}

void MainMenu::update()
{
	if (lazy::inputs::input::getMouse().getButton(0) == GLFW_PRESS) {
		_onStartPlaying();
	}
}

void MainMenu::render()
{
	_shader.bind();
	TextureManager::instance().bind("MenuBackground", GL_TEXTURE0);
	_background.draw();
	_shader.unbind();

	tr.drawText("Click to Start Playing", glm::vec2(10, 10), .5f, glm::vec3(1.0f, 1.0f, 1.0f));
}
