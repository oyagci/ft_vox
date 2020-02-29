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
		_button->setAnchor(Anchor::Center);
		_button->setOrigin(Origin::Center);
		_button->setText("Singleplayer");
		_button->_onClick = [this] { call("playSingleplayer"); };
	};
	~MainMenuScene() {};

	void update() override {};

private:
	std::shared_ptr<Button> _button;
};
