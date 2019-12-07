#include "ChunkFactory.hpp"
#include <glm/vec2.hpp>

ChunkFactory::ChunkFactory()
{
}

std::unique_ptr<Chunk> ChunkFactory::getChunk(glm::vec3 pos)
{
	glm::i32vec2 finalPos(pos.x, pos.z);
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(finalPos);

	return chunk;
}
