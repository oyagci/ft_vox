#pragma once

#include "lazy.hpp"
#include "TextRenderer.hpp"

using lazy::graphics::Mesh;

class Button
{
public:
	enum class Anchor {
		TopLeft,
		TopCenter,
		TopRight,
		Left,
		Center,
		Right,
		BottomLeft,
		BottomCenter,
		BottomRight,
	};
public:
	Button(glm::vec2 position, glm::vec2 size, std::function<void()> onClick,
		Anchor anchorPoint = Anchor::Center);

	void update();
	void draw();

	void setText(std::string const &text);

	glm::vec2 getPosition() const { return _position; }
	glm::vec2 getSize() const { return _size; }

	bool isInside(glm::vec2 pos) {
		return pos.x >= _position.x && pos.x <= _position.x + _size.x &&
			pos.y >= _position.y && pos.y <= _position.y + _size.y;
	}

private:
	Mesh _mesh;
	glm::vec2 _position;
	glm::vec2 _size;
	std::function<void()> _onClick;
	std::string _text;
	TextRenderer _textRenderer;
};
