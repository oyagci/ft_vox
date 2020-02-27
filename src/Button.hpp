#pragma once

#include "lazy.hpp"
#include "TextRenderer.hpp"
#include "Anchor.hpp"
#include "SceneComponent.hpp"

using lazy::graphics::Mesh;
using namespace anchor;

class Button : public lazy::inputs::IMouseObserver, public ASceneComponent
{
public:
	Button(glm::vec2 position, glm::vec2 size, std::function<void()> onClick,
		Anchor anchorPoint = Anchor::Center);

	void update() override;
	void draw() override;

	void setText(std::string const &text);

	glm::vec2 getPosition() const override { return _position; }
	glm::vec2 getSize() const override { return _size; }

	// IMouseObserver member functions
	void onClickUpInside() override;
	void onHover(bool) override;
	glm::vec4 getObservedArea() const override;
	bool canBeClicked() const override { return _canBeClicked; }
	void setCanBeClicked(bool val) { _canBeClicked = val; }

private:
	Mesh _mesh;
	glm::vec2 _position;
	glm::vec2 _size;
	std::function<void()> _onClick;
	std::string _text;
	Anchor _anchor;
	bool _canBeClicked;
	bool _isHovering;
};
