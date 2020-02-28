#pragma once

#include "SceneComponent.hpp"
#include "TextRenderer.hpp"

class Label : public ASceneComponent
{
public:
	Label() : _tr(1920.0f, 1080.0f), _scale(0.6f)
	{
		_onClick = [this] { call("singleplayerStart"); };
	}

	void update() override {};

	void draw() override {
		_tr.drawText(_text, _position, _scale, glm::vec3(1.0f, 1.0f, 1.0f), Anchor::Center);
	};

	glm::vec2 getPosition() const override
	{
		return _position;
	}

	glm::vec2 getSize() const override
	{
		return glm::vec2(0.0f, 0.0f);
	}

	void setPosition(glm::vec2 position) { _position = position; }
	void setText(std::string const &text) { _text = text; }

private:
	std::string _text;
	TextRenderer _tr;
	glm::vec2 _position;
	float _scale;
	std::function<void()> _onClick;
};
