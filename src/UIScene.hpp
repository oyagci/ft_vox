#pragma once

#include <memory>
#include "SceneComponent.hpp"
#include <type_traits>

class IUIScene
{
public:
	virtual ~IUIScene() {};
	virtual void update() = 0;

	std::vector<std::shared_ptr<ASceneComponent>> getSceneComponents()
	{
		return _sceneComponents;
	}

	template<typename T, typename... Args,
	typename = std::enable_if_t<std::is_base_of<ASceneComponent, T>::value>>
	std::shared_ptr<T> createSceneComponent(Args... args)
	{
		auto component = std::make_shared<T>(std::forward<Args>(args)...);

		_sceneComponents.push_back(component);
		return component;
	}

private:
	std::vector<std::shared_ptr<ASceneComponent>> _sceneComponents;
};
