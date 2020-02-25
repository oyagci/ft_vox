#include "UI.hpp"
#include <iostream>
#include "MainMenuScene.hpp"

UI::UI()
{
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
}
