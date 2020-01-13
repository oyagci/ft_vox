#pragma once

#include "Chunk.hpp"

class WorldRenderer;

class ChunkFactory
{
public:
	ChunkFactory(WorldRenderer *wr);

	std::unique_ptr<Chunk> getChunk(glm::vec3 pos);

private:
	WorldRenderer *_worldRenderer;
};
