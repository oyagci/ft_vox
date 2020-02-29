#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Anchor.hpp"

using namespace anchor;
using Origin = Anchor;

class IUIScene;

class ASceneComponent
{
public:
	ASceneComponent() = delete;
	ASceneComponent(IUIScene *parent) : _scene(parent), _anchor(Anchor::Center), _origin(Origin::Center)
	{}

	virtual ~ASceneComponent() {}
	virtual void update() = 0;
	virtual void draw() = 0;

	glm::vec2 getSize() const { return _size; }
	Anchor getAnchor() const { return _anchor; }
	Origin getOrigin() const { return _origin; }
	glm::vec2 getScreenPosition() const;

	void setSize(glm::vec2 size);
	void setOrigin(Origin origin) { _origin = origin; }
	void setAnchor(Anchor anchor) { _anchor = anchor; }

	std::vector<std::shared_ptr<ASceneComponent>> &getSubComponents() {
		return _subComponents;
	}

protected:
	template<typename T, typename... Args,
	typename = std::enable_if_t<std::is_base_of<ASceneComponent, T>::value>>
	std::shared_ptr<T> createSubComponent(Args... args)
	{
		auto component = std::make_shared<T>(_scene, std::forward<Args>(args)...);

		_subComponents.push_back(component);
		return component;
	}

	/*
	 * Call a function registered to the UI
	 */
	void call(std::string const &funcName);

private:
	std::vector<std::shared_ptr<ASceneComponent>> _subComponents;
	IUIScene *_scene;
	glm::vec2 _size;
	Anchor _anchor;
	Origin _origin;
};
