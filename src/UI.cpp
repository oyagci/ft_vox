#include "UI.hpp"
#include <iostream>
#include "MainMenuScene.hpp"

UI::UI()
{
	_shader.addVertexShader("shaders/ui.vs.glsl")
		.addFragmentShader("shaders/ui.fs.glsl")
		.link();

	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix", glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f));
	_shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
	_shader.unbind();

	loadScene<MainMenuScene>("mainMenu");
	action(CHANGE_SCENE, "mainMenu");
}

void UI::update()
{
	_state.currentScene->update();
}

void UI::render()
{
	renderScene(_state.currentScene);
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
	std::shared_ptr<T> scene = std::make_shared<T>();
	_scenes[name] = scene;

	return true;
}

void UI::renderScene(std::shared_ptr<IUIScene> scene)
{
	_shader.bind();
	renderComponents(scene->getSceneComponents());
	_shader.unbind();
}

void UI::renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components)
{
	for (auto &c : components) {
		glm::vec3 position(c->getPosition(), 0.0f);
		glm::mat4 modelMatrix(1.0f);

		modelMatrix = glm::translate(modelMatrix, position);
		_shader.setUniform4x4f("modelMatrix", modelMatrix);

		c->draw();
		renderComponents(c->getSubComponents());
	}
}
