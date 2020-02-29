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
		_spButton = createSceneComponent<Button>();
		_spButton->setAnchor(Anchor::Center);
		_spButton->setOrigin(Origin::Center);
		_spButton->setText("Singleplayer");
		_spButton->_onClick = [this] { call("playSingleplayer"); };
	};
	~MainMenuScene() {};

	void update() override {};

private:
	std::shared_ptr<Button> _spButton;
};
