#include "ChunkFactory.hpp"
#include <glm/vec2.hpp>
#include "World.hpp"

ChunkFactory::ChunkFactory(World *world) : _world(world)
{
}

std::unique_ptr<Chunk> ChunkFactory::getChunk(glm::vec3 pos)
{
	glm::ivec2 finalPos(pos.x, pos.z);
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(finalPos, _world);

	return chunk;
}
