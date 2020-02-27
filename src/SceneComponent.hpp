#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <memory>

class ASceneComponent
{
public:
	virtual ~ASceneComponent() {}
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual glm::vec2 getPosition() const = 0;
	virtual glm::vec2 getSize() const = 0;

	std::vector<std::shared_ptr<ASceneComponent>> &getSubComponents() {
		return _subComponents;
	}

	template<class T>
	std::shared_ptr<ASceneComponent> createSubComponent()
	{
		auto component = std::make_shared<T>();

		_subComponents.push_back(component);
		return component;
	}

private:
	std::vector<std::shared_ptr<ASceneComponent>> _subComponents;
};
