#pragma once

#include <vector>
#include <memory>
#include "UIScene.hpp"
#include "SceneComponent.hpp"
#include "Button.hpp"

class MainMenuScene : public IUIScene
{
public:
	MainMenuScene() {
		auto button = std::make_shared<Button>(
			glm::vec2(1920.0f / 2.0f, 1080.0f / 2.0f),
			glm::vec2(640.0f, 64.0f), [] {}, Anchor::Center);

		_button = createSceneComponent<Button>();
	};
	~MainMenuScene() {};

	void update() override {};

private:
	std::shared_ptr<Button> _button;
};
