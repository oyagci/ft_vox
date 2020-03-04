#pragma once

#include "UI.hpp"
#include "Image.hpp"
#include "Hotbar.hpp"

class PlayerHUDScene : public IUIScene
{
public:
	PlayerHUDScene(UI *uiController, glm::vec2 size) : IUIScene(uiController, size)
	{
		_hotbar = createSceneComponent<Hotbar>();
		_hotbar->setSize(getSize());

		uiController->registerFunc("setHotbarItem",
			Callback<int>([this] (int index) { _hotbar->setSlot(index); }));
	}
private:
	std::shared_ptr<Hotbar> _hotbar;
};
