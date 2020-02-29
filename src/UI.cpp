#include "UI.hpp"
#include <iostream>
#include "MainMenuScene.hpp"
#include "PlayerHUDScene.hpp"

UI::UI(float width, float height)
{
	_shader.addVertexShader("shaders/ui.vs.glsl")
		.addFragmentShader("shaders/ui.fs.glsl")
		.link();

	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix", glm::ortho(0.0f, width, 0.0f, height));
	_shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
	_shader.unbind();

	loadScene<MainMenuScene>("mainMenu");
	loadScene<PlayerHUDScene>("playerHud");
	action(CHANGE_SCENE, "mainMenu");
}

void UI::update()
{
	_state.currentScene->update();
}

void UI::render()
{
	renderScene(*_state.currentScene);
}

void UI::action(UIAction action, std::string const &val)
{
	std::optional<std::shared_ptr<IUIScene>> scene;

	switch (action)
	{
	case CHANGE_SCENE:
		scene = getScene(val);
		if (scene.has_value()) {
			_state.currentScene = scene.value();
		}
		break ;
	default:
		break ;
	};
}

auto UI::getScene(std::string const &name) -> std::optional<std::shared_ptr<IUIScene>>
{
	std::optional<std::shared_ptr<IUIScene>> scene;

	auto sceneIter = _scenes.find(name);
	if (sceneIter != _scenes.end()) {
		scene = sceneIter->second;
	}
	return scene;
}

template<class T>
bool UI::loadScene(std::string const &name)
{
	std::shared_ptr<T> scene = std::make_shared<T>(this);
	_scenes[name] = scene;

	return true;
}

void UI::renderScene(IUIScene &scene)
{
	_shader.bind();
	renderComponents(scene.getSceneComponents());
	_shader.unbind();
}

void UI::renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components)
{
	for (auto &c : components) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(c->getScreenPosition(), 0.0f));

		_shader.setUniform4x4f("modelMatrix", modelMatrix);

		c->draw();
		renderComponents(c->getSubComponents());
	}
}

void UI::call(const std::string &name)
{
	if (_callbacks.find(name) == _callbacks.end()) { return ; }

	_callbacks[name]();
}

void UI::registerFunc(std::string const &name, std::function<void()> cb)
{
	if (_callbacks.find(name) != _callbacks.end()) { return ; }

	_callbacks[name] = cb;
}

void UI::showScene(std::string const &sceneName)
{
	action(CHANGE_SCENE, sceneName);
}
