#pragma once

#include "SceneComponent.hpp"
#include "TextRenderer.hpp"

class Label : public ASceneComponent
{
public:
	Label(IUIScene *scene) : ASceneComponent(scene), _tr(2560.0f, 1440.0f), _scale(0.6f)
	{}

	void update() override {};

	void draw() override {
		_tr.drawText(_text, getScreenPosition() + glm::vec2(2.0f, -2.0f), _scale, glm::vec3(0.0f, 0.0f, 0.0f),
			Anchor::Center);
		_tr.drawText(_text, getScreenPosition(), _scale, glm::vec3(1.0f, 1.0f, 1.0f), Anchor::Center);
	};

	void setText(std::string const &text) { _text = text; }

private:
	std::string _text;
	TextRenderer _tr;
	float _scale;
};
