#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <memory>

class IUIScene;

class ASceneComponent
{
public:
	ASceneComponent() = delete;
	ASceneComponent(IUIScene *parent) : _scene(parent) {}

	virtual ~ASceneComponent() {}
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual glm::vec2 getPosition() const = 0;
	virtual glm::vec2 getSize() const = 0;

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
};
