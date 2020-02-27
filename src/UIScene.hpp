#pragma once

#include "SceneComponent.hpp"

class IUIScene
{
public:
	virtual ~IUIScene() {};
	virtual void update() = 0;

	std::vector<std::shared_ptr<ASceneComponent>> getSceneComponents()
	{
		return _sceneComponents;
	}

	template<class T>
	std::shared_ptr<ASceneComponent> createSceneComponent()
	{
		auto component = std::make_shared<T>();

		_sceneComponents.push_back(component);
		return component;
	}

private:
	std::vector<std::shared_ptr<ASceneComponent>> _sceneComponents;
};
