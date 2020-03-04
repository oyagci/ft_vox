#include "UI.hpp"
#include <iostream>
#include "MainMenuScene.hpp"
#include "PlayerHUDScene.hpp"
#include "UIScene.hpp"

UI::UI(int width, int height)
{
	_shader.addVertexShader("shaders/ui.vs.glsl")
		.addFragmentShader("shaders/ui.fs.glsl")
		.link();

	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix", glm::ortho(0.0f, (float)width, 0.0f, (float)height));
	_shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
	_shader.unbind();

	_size = glm::vec2(width, height);

	loadScene<MainMenuScene>("mainMenu");
	loadScene<PlayerHUDScene>("playerHud");
	action(CHANGE_SCENE, "mainMenu");
}

void UI::update()
{
	_state.currentScene->update();
	updateComponents(_state.currentScene->getSceneComponents());
}

void UI::updateComponents(std::vector<std::shared_ptr<ASceneComponent>> components)
{
	for (auto &c : components)
	{
		c->update();
		updateComponents(c->getSubComponents());
	}
}

void UI::render()
{
	renderScene(*_state.currentScene);
}

void UI::action(UIAction action, std::string const &val)
{
	std::optional<std::shared_ptr<IUIScene>> scene;

	switch (action)
	{
	case CHANGE_SCENE:
		scene = getScene(val);
		if (scene.has_value()) {
			_state.currentScene = scene.value();
		}
		break ;
	default:
		break ;
	};
}

auto UI::getScene(std::string const &name) -> std::optional<std::shared_ptr<IUIScene>>
{
	std::optional<std::shared_ptr<IUIScene>> scene;

	auto sceneIter = _scenes.find(name);
	if (sceneIter != _scenes.end()) {
		scene = sceneIter->second;
	}
	return scene;
}

void UI::renderScene(IUIScene &scene)
{
	_shader.bind();
	renderComponents(scene.getSceneComponents());
	_shader.unbind();
}

void UI::renderComponents(std::vector<std::shared_ptr<ASceneComponent>> components,
	ASceneComponent *parent, glm::vec2 parentPos)
{
	for (auto &c : components) {
		glm::vec2 position(0.0f);
		glm::vec2 anchorOff = calculateOffset(c->getAnchor(), c->getSize());
		glm::mat4 modelMatrix(1.0f);

		// TODO: Do this switch in ASceneComponent::getScreenPosition()
		//       (getScreenPosition needs to access the parent's size)
		glm::vec2 parentSize = parent ? parent->getSize() : _size;

		switch (c->getOrigin()) {
		case Origin::TopLeft:
			position = glm::vec2(0.0f, parentSize.y);
			break ;
		case Origin::Top:
			position = glm::vec2(parentSize.x / 2.0f, parentSize.y);
			break ;
		case Origin::TopRight:
			position = glm::vec2(parentSize.x, parentSize.y);
			break ;
		case Origin::Left:
			position = glm::vec2(0.0f, parentSize.y / 2.0f);
			break ;
		case Origin::Right:
			position = glm::vec2(parentSize.x, parentSize.y / 2.0f);
			break ;
		case Origin::Center:
			position = parentSize / 2.0f;
			break ;
		case Origin::BottomLeft:
			position = glm::vec2(0.0f, 0.0f);
			break ;
		case Origin::Bottom:
			position = glm::vec2(parentSize.x / 2.0f, 0.0f);
			break ;
		case Origin::BottomRight:
			position = glm::vec2(parentSize.x, 0.0f);
			break ;
		default:
			break ;
		};

		auto offset = c->getOffset();
		position += anchorOff;
		position += offset;

		modelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(parentPos + position, 0.0f));

		_shader.bind();
		_shader.setUniform4x4f("modelMatrix", modelMatrix);

		c->draw(_shader);

		renderComponents(c->getSubComponents(), c.get(), position);
	}
}

void UI::showScene(std::string const &sceneName)
{
	action(CHANGE_SCENE, sceneName);
}
