#pragma once

#include "ecs/Component.hpp"
#include <glm/vec3.hpp>

struct Position : public ecs::IComponent
{
	Position() : position(glm::vec3(0.0f)) {}
	Position(glm::vec3 p) : position(p) {}
	glm::vec3 position;
};
