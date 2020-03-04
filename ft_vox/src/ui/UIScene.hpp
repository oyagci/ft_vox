#pragma once

#include <memory>
#include "SceneComponent.hpp"
#include <type_traits>
#include <functional>

class UI;

class IUIScene
{
public:
	IUIScene() = delete;
	IUIScene(UI *uiController, glm::vec2 size) {
		_uiController = uiController;
		_size = size;
	}

	virtual ~IUIScene() {};
	virtual void update() {};

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

	template <typename ... Args>
	void call(std::string const &name, Args... args)
	{
		_uiController->call(name, std::forward<Args>(args)...);
	}

	template <typename ... Args>
	void registerFunc(std::string const &name, std::function<void(Args...)> f)
	{
		_uiController->registerFunc(name, f);
	}

	glm::vec2 getSize() { return _size; };

private:
	std::vector<std::shared_ptr<ASceneComponent>> _sceneComponents;
	UI *_uiController;
	glm::vec2 _size;
};
