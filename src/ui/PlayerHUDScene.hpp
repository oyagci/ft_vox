#pragma once

#include "UIScene.hpp"

class PlayerHUDScene : public IUIScene
{
public:
	PlayerHUDScene(UI *uiController, glm::vec2 size) : IUIScene(uiController, size)
	{
	}

	void update() override
	{
	}
private:
};
