#pragma once

#include <string>
#include <map>
#include <optional>
#include <memory>
#include <type_traits>
#include "SceneComponent.hpp"
#include "lazy.hpp"
#include <functional>
#include "UIScene.hpp"

using lazy::graphics::Shader;

class BaseCallback
{
public:
	virtual ~BaseCallback() {}
};


template <typename ... Args>
class Callback : public BaseCallback
{
private:
	typedef std::function<void(Args...)> FuncType;
	FuncType _f;
public:
	Callback() {}
	Callback(FuncType f) { _f = f; }
	void operator()(Args... args) { if (_f) { _f(args...); } }
};

class UI
{
private:
	struct UIState {
		std::string sceneName;
		std::shared_ptr<IUIScene> currentScene;
	};

	enum UIAction {
		CHANGE_SCENE,
	};
public:
	UI(int width, int height);

	void update();
	void render();

	/*
	 * Call a registered callback from a UI component or the game
	 */
	template <typename... Args>
	void call(const std::string &name, Args... args)
	{
		if (_callbacks.find(name) == _callbacks.end()) { return ; }

		typedef Callback<Args...> CallbackType;

		CallbackType *cb = dynamic_cast<CallbackType*>(_callbacks[name].get());

		if (cb) {
			(*cb)(args...);
		}
	}

	/*
	 * Register a callback to be used by a UI component or the game
	 */
	template <typename T,
	typename = std::enable_if_t<std::is_base_of<BaseCallback, T>::value>>
	void registerFunc(std::string const &name, const T &cb)
	{
		if (_callbacks.find(name) != _callbacks.end()) { return ; }

		_callbacks.insert(std::pair<std::string, std::shared_ptr<BaseCallback>>(name, std::make_shared<T>(cb)));
	}

	void showScene(std::string const &sceneName);

private:
	void action(UIAction action, std::string const &);

	std::optional<std::shared_ptr<IUIScene>> getScene(std::string const &name);

	template<class T,
	typename = std::enable_if_t<std::is_base_of<IUIScene, T>::value>>
	bool loadScene(std::string const &name)
	{
		std::shared_ptr<T> scene = std::make_shared<T>(this, _size);
		_scenes[name] = scene;

		return true;
	}

	void renderScene(IUIScene &scene);

	void renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components,
		ASceneComponent *parent = nullptr, glm::vec2 parentPos = glm::vec2(0.0f, 0.0f));

	void updateComponents(std::vector<std::shared_ptr<ASceneComponent>> components);

private:

	UIState _state;
	std::map<std::string, std::shared_ptr<IUIScene>> _scenes;
	Shader _shader;

	std::map<std::string, std::shared_ptr<BaseCallback>> _callbacks;

	glm::vec2 _size;
};
