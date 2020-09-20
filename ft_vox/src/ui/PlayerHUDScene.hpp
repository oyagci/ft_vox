#pragma once

#include "UIScene.hpp"
#include "Image.hpp"

class PlayerHUDScene : public IUIScene
{
public:
	PlayerHUDScene(UI *uiController, glm::vec2 size) : IUIScene(uiController, size)
	{
	}
private:
};
