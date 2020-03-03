#pragma once

#include "SceneComponent.hpp"
#include "Image.hpp"

class Hotbar : public ASceneComponent
{
public:
	Hotbar(IUIScene *parent) : ASceneComponent(parent)
	{
		_slotsBackground = createSubComponent<Image>();
		_slotsBackground->setImage(546.0f * 1.0f, 66.0f * 1.0f, "img/hotbar_full.png");
		_slotsBackground->setAnchor(Anchor::Bottom);
		_slotsBackground->setOrigin(Origin::Bottom);
		_slotsBackground->setOffset(glm::vec2(0.0f, 0.0f));
	}

	void draw(Shader &shader) override
	{
	}

private:
	std::shared_ptr<Image> _slotsBackground;
};
