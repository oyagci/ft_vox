#pragma once

#include <string>
#include <map>
#include <optional>
#include <memory>
#include "UIScene.hpp"
#include "lazy.hpp"

using lazy::graphics::Shader;

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
	UI();

	void update();
	void render();

	void action(UIAction action, std::string const &);

private:
	std::optional<std::shared_ptr<IUIScene>> getScene(std::string const &name);

	template<class T>
	bool loadScene(std::string const &name);

	void renderScene(std::shared_ptr<IUIScene> scene);
	void renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components);

	UIState _state;
	std::map<std::string, std::shared_ptr<IUIScene>> _scenes;

	Shader _shader;
};
