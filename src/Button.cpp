#include "Button.hpp"

Button::Button(glm::vec2 position, glm::vec2 size, std::function<void()> onClick, Anchor anchorPoint) :
	_onClick(onClick)
{
	glm::vec3 anchorOffset(0.0f, 0.0f, 0.0f);

	switch (anchorPoint) {
	case Anchor::TopLeft:
		anchorOffset = -glm::vec3(0.0f, size.y, 0.0f);
		break ;
	case Anchor::TopCenter:
		anchorOffset = -glm::vec3(size.x / 2.0f, size.y, 0.0f);
		break ;
	case Anchor::TopRight:
		anchorOffset = -glm::vec3(size.x, size.y, 0.0f);
		break ;
	case Anchor::Left:
		anchorOffset = -glm::vec3(0.0f, size.y / 2.0f, 0.0f);
		break ;
	case Anchor::Center:
		anchorOffset = -glm::vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);
		break ;
	case Anchor::Right:
		anchorOffset = -glm::vec3(size.x, size.y / 2.0f, 0.0f);
		break ;
	case Anchor::BottomLeft:
		anchorOffset = -glm::vec3(0.0f, 0.0f, 0.0f);
		break ;
	case Anchor::BottomCenter:
		anchorOffset = -glm::vec3(size.x / 2.0f, 0.0f, 0.0f);
		break ;
	case Anchor::BottomRight:
		anchorOffset = -glm::vec3(size.x, 0.0f, 0.0f);
		break ;
	}

	_position = position + glm::vec2(anchorOffset.x, anchorOffset.y);
	_size = size;

	glm::vec3 quad[] = {
		glm::vec3(position.x,          position.y,          0.0f),
		glm::vec3(position.x + size.x, position.y,          0.0f),
		glm::vec3(position.x + size.x, position.y + size.y, 0.0f),
		glm::vec3(position.x,          position.y + size.y, 0.0f),
	};
	glm::uvec3 inds[] = {
		glm::uvec3(0, 1, 2),
		glm::uvec3(0, 2, 3),
	};
	glm::vec2 tex[] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
	};

	for (auto const &p : quad) {
		_mesh.addPosition(p + anchorOffset);
	}
	for (auto const &i : inds) {
		_mesh.addTriangle(i);
	}
	for (auto const &t : tex) {
		_mesh.addUv(t);
	}
	_mesh.build();
}

void Button::update()
{
}

void Button::draw()
{
	TextureManager::instance().bind("Button", GL_TEXTURE0);
	_mesh.draw();
	_textRenderer.drawText(_text, _position + glm::vec2(16.0f, 7.0f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
}

void Button::setText(std::string const &text)
{
	_text = text;
}
