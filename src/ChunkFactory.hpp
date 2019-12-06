#pragma once

#include "Chunk.hpp"

class ChunkFactory
{
public:
	ChunkFactory();

	std::unique_ptr<Chunk> getChunk();

private:
	glm::vec2 _nextChunkPos;
};
