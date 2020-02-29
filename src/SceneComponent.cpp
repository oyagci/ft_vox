#include "SceneComponent.hpp"
#include "UI.hpp"
#include "UIScene.hpp"

void ASceneComponent::call(std::string const &funcName)
{
	_scene->call(funcName);
}

void ASceneComponent::setSize(glm::vec2 size)
{
	_size = size;
}

glm::vec2 ASceneComponent::getScreenPosition() const
{
	glm::vec2 position(0.0f);
	glm::vec2 anchorOff = calculateOffset(getAnchor(), getSize());
	glm::mat4 modelMatrix(1.0f);

	switch (getOrigin()) {
	case Origin::Center:
		position = glm::vec2(2560.0f, 1440.0f) / 2.0f;
		break ;
	default:
		break ;
	};

	position += anchorOff;

	modelMatrix = glm::translate(modelMatrix, glm::vec3(position, 0.0f));
	glm::vec3 screenPosition = glm::vec4(position, 0.0f, 1.0f) * modelMatrix;

	return screenPosition;
}
