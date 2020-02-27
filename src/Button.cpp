#include "Button.hpp"
#include "TextureManager.hpp"
#include "Anchor.hpp"
#include "TextComponent.hpp"

Button::Button(glm::vec2 position, glm::vec2 size, std::function<void()> onClick, Anchor anchorPoint) :
	_onClick(onClick), _anchor(anchorPoint), _canBeClicked(true)
{
	TextureManager::instance().createTexture("Button", "img/button.png", {
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	});
	TextureManager::instance().createTexture("ButtonHovering", "img/button_hover.png", {
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST }
	});

	glm::vec2 anchorOffset = calculateOffset(anchorPoint, size);

	_position = position + anchorOffset;
	_size = size;

	glm::vec3 quad[] = {
		glm::vec3(0.0f,          0.0f,          0.0f),
		glm::vec3(0.0f + size.x, 0.0f,          0.0f),
		glm::vec3(0.0f + size.x, 0.0f + size.y, 0.0f),
		glm::vec3(0.0f,          0.0f + size.y, 0.0f),
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
		_mesh.addPosition(p);
	}
	for (auto const &i : inds) {
		_mesh.addTriangle(i);
	}
	for (auto const &t : tex) {
		_mesh.addUv(t);
	}
	_mesh.build();

	lazy::inputs::input::getMouse().attach(this);

	auto textComponent = createSubComponent<TextComponent>();
	//std::make_shared<TextComponent>(_text, _position + (_size / 2.0f), 0.6f, glm::vec3(1.0f, 1.0f, 1.0f), _anchor);
}

void Button::update()
{
}

void Button::draw()
{
	if (_isHovering) {
		TextureManager::instance().bind("ButtonHovering", GL_TEXTURE0);
	}
	else {
		TextureManager::instance().bind("Button", GL_TEXTURE0);
	}
	_mesh.draw();
}

void Button::setText(std::string const &text)
{
	_text = text;
}

void Button::onClickUpInside()
{
	_canBeClicked = false;
	_onClick();
}

void Button::onHover(bool val)
{
	_isHovering = val;
}

glm::vec4 Button::getObservedArea() const
{
	return glm::vec4(_position, _size);
}
