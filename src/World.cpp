#include "World.hpp"

World::World(Camera &cam, glm::vec3 &camPos)
{
	_renderer = std::make_unique<WorldRenderer>(cam, camPos);
}

void World::render()
{
	_renderer->render();
}
