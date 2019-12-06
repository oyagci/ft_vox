#include "ChunkFactory.hpp"
#include <glm/vec2.hpp>

ChunkFactory::ChunkFactory() : _nextChunkPos(std::move(glm::vec2()))
{
}

std::unique_ptr<Chunk> ChunkFactory::getChunk()
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(_nextChunkPos);

	_nextChunkPos += glm::vec2(_nextChunkPos.x + 64.0f, _nextChunkPos.y + 64.0f);

	return chunk;
}
