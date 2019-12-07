#pragma once

#include "Chunk.hpp"

class ChunkFactory
{
public:
	ChunkFactory();

	std::unique_ptr<Chunk> getChunk(glm::vec3 pos);
};
