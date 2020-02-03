#include "World.hpp"
#include "Settings.hpp"

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
	auto chunksTooFar = getChunksTooFar(camPos);

	_renderer->removeChunksTooFar(chunksTooFar);

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

	removeChunksTooFar(chunksTooFar);
}

std::vector<glm::vec2> World::getChunksTooFar(glm::vec3 camPos)
{
	glm::vec2 camPos2D(camPos.x, camPos.z);
	int renderDistance = std::any_cast<int>(Settings::instance().get("renderDistance"));
	std::vector<glm::vec2> positions;

	for (auto &c : _chunks) {
		float dist = glm::length(c->getPosition() - camPos2D);

		if (dist / 64 > renderDistance) {
			positions.push_back(c->getPosition());
		}
	}

	return positions;
}

void World::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	std::list<std::shared_ptr<Chunk>> chunks;
	int n = 0;

	for (auto &c : _chunks) {
		bool remove = false;
		for (auto const &pos : chunksTooFar) {
			if (c->getPosition() == pos) {
				remove = true;
				n++;
			}
		}
		if (!remove) {
			chunks.push_back(c);
		}
	}

	_chunks = chunks;
}
