#pragma once

#include "Chunk.hpp"

class World;

class ChunkFactory
{
public:
	ChunkFactory(World *world);
	std::unique_ptr<Chunk> getChunk(glm::vec3 pos);
private:
	World *_world;
};
