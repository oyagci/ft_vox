#pragma once

#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "Anchor.hpp"

using lazy::graphics::Mesh;
using namespace anchor;

class Button
{
public:
	Button(glm::vec2 position, glm::vec2 size, std::function<void()> onClick,
		Anchor anchorPoint = Anchor::Center);

	void update();
	void draw();

	void setText(std::string const &text);

	glm::vec2 getPosition() const { return _position; }
	glm::vec2 getSize() const { return _size; }
	bool isInside(glm::vec2 pos) const;

private:
	Mesh _mesh;
	glm::vec2 _position;
	glm::vec2 _size;
	std::function<void()> _onClick;
	std::string _text;
	TextRenderer _textRenderer;
	Anchor _anchor;
};
