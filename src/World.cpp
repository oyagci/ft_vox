#include "World.hpp"

World::World(Camera &cam, glm::vec3 &camPos) : _camera(camPos)
{
	_renderer = std::make_unique<WorldRenderer>(cam, camPos);
	_builder = std::make_unique<WorldGenerator>(_renderer.get());
}

void World::render()
{
	_renderer->render();
}

void World::update(glm::vec3 camPos)
{
	_builder->setCameraPosition(camPos);
	_builder->update();
	auto chunks = _builder->takeChunks();
	if (!chunks.empty()) {
		// New chunks have been created
		// Add them to list of chunks
		_chunks.insert(_chunks.end(), chunks.begin(), chunks.end());

		// Register the new chunks to the renderer
		_renderer->registerChunks(chunks);
	}

	_renderer->update();
}
