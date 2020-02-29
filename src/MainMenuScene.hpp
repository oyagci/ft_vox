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

		_quitButton = createSceneComponent<Button>();
		_quitButton->setAnchor(Anchor::Center);
		_quitButton->setOrigin(Origin::Center);
		_quitButton->setText("Quit Game");
		_quitButton->setOffset(glm::vec2(0.0f, -64.0f - 8.0f));
		_quitButton->_onClick = [this] { call("quitGame"); };

		_titleLabel = createSceneComponent<Label>();
		_titleLabel->setText("ft_vox");
		_titleLabel->setAnchor(Anchor::Top);
		_titleLabel->setOrigin(Origin::Top);
		_titleLabel->setScale(2.5f);
		_titleLabel->setOffset(glm::vec2(0.0f, -150.0f));
	};
	~MainMenuScene() {};

	void update() override {};

private:
	std::shared_ptr<Label> _titleLabel;
	std::shared_ptr<Button> _spButton;
	std::shared_ptr<Button> _quitButton;
};
