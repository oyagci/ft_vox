#pragma once

#include "ecs/Component.hpp"

struct Speed : public ecs::IComponent
{
	/* Range should be between -1.0f and 1.0f */
	float forward;

	/* Range should be between -1.0f and 1.0f */
	float right;
};
