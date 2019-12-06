#include "ChunkFactory.hpp"
#include <glm/vec2.hpp>

ChunkFactory::ChunkFactory() : _nextChunkPos(std::move(glm::vec2()))
{
}

std::unique_ptr<Chunk> ChunkFactory::getChunk()
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(_nextChunkPos);

	if (_nextChunkPos.x > 512) {
		_nextChunkPos += glm::vec2(0.0f, 64.0f);
		_nextChunkPos.x = 0.0f;
	}
	else {
		_nextChunkPos += glm::vec2(64.0f, 0.0f);
	}

	return chunk;
}
