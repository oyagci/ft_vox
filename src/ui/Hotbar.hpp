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

		_slotIndex = 0;

		_firstSlotOffset = -60.0f * 4;

		_slotCurrent = createSubComponent<Image>();
		_slotCurrent->setImage(72.0f, 69.0f, "img/hotbar_selected.png");
		_slotCurrent->setAnchor(Anchor::Bottom);
		_slotCurrent->setOrigin(Origin::Bottom);
		_slotCurrent->setOffset(glm::vec2(_firstSlotOffset, 0.0f));
	}

	void draw(Shader &shader) override
	{
		(void)shader;
	}

	void setSlot(int index)
	{
		if (index < 0) { index = 9; }
		if (index >= 9) { index = 0; }

		_slotIndex = index;
		_slotCurrent->setOffset(glm::vec2(_firstSlotOffset + 60.0f * _slotIndex, 0.0f));
	}

private:
	std::shared_ptr<Image> _slotsBackground;
	std::shared_ptr<Image> _slotCurrent;
	float _firstSlotOffset;
	int _slotIndex;
};
