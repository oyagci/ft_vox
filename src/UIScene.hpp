#pragma once

#include <memory>
#include "SceneComponent.hpp"
#include <type_traits>

class UI;

class IUIScene
{
public:
	IUIScene() = delete;
	IUIScene(UI *uiController) { _uiController = uiController; }

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
		auto component = std::make_shared<T>(this, std::forward<Args>(args)...);

		_sceneComponents.push_back(component);
		return component;
	}

	void call(std::string const &name);

private:
	std::vector<std::shared_ptr<ASceneComponent>> _sceneComponents;
	UI *_uiController;
};
