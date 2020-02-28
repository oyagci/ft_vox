#pragma once

#include "SceneComponent.hpp"
#include "Anchor.hpp"
#include <glm/glm.hpp>
#include <string>

using anchor::Anchor;

class TextComponent : public ASceneComponent
{
public:
	TextComponent(IUIScene *scene) : ASceneComponent(scene)
	{
	}

	void update() override
	{
	}

	void draw() override
	{
	}

	glm::vec2 getPosition() const override
	{
		return glm::vec2(0.0f, 0.0f);
	}

	glm::vec2 getSize() const override
	{
		return glm::vec2(0.0f, 0.0f);
	}
};
