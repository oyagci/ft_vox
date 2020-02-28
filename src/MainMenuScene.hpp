#pragma once

#include <vector>
#include <memory>
#include "UIScene.hpp"
#include "SceneComponent.hpp"
#include "Button.hpp"

class UI;

class MainMenuScene : public IUIScene
{
public:
	MainMenuScene() = delete;
	MainMenuScene(UI *uiController) : IUIScene(uiController) {
		_button = createSceneComponent<Button>();
		_button->setPosition(glm::vec2(1920.0f / 2.0f, 1080.0f / 2.0f));
		_button->setText("Singleplayer");
	};
	~MainMenuScene() {};

	void update() override {};

private:
	std::shared_ptr<Button> _button;
};
