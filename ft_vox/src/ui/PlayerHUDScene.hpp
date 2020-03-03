#pragma once

#include "UIScene.hpp"
#include "Image.hpp"
#include "Hotbar.hpp"

class PlayerHUDScene : public IUIScene
{
public:
	PlayerHUDScene(UI *uiController, glm::vec2 size) : IUIScene(uiController, size)
	{
		_hotbar = createSceneComponent<Hotbar>();
		_hotbar->setSize(getSize());
	}
private:
	std::shared_ptr<Hotbar> _hotbar;
};
