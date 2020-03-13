#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>

struct Direction : public ecs::IComponent
{
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
};
