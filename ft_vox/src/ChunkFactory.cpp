#include "ChunkFactory.hpp"
#include <glm/vec2.hpp>
#include "World.hpp"

ChunkFactory::ChunkFactory(World *world, unsigned int seed) : _world(world), _seed(seed)
{
}

std::unique_ptr<ChunkController> ChunkFactory::getChunk(glm::vec3 pos)
{
	glm::ivec2 finalPos(pos.x, pos.z);
	std::unique_ptr<ChunkController> chunk = std::make_unique<ChunkController>(_seed, finalPos, _world);

	return chunk;
}
