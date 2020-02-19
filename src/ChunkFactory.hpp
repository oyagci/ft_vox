#pragma once

#include "Chunk.hpp"

class World;

class ChunkFactory
{
public:
	ChunkFactory(World *world, unsigned int seed);
	std::unique_ptr<Chunk> getChunk(glm::vec3 pos);
private:
	World *_world;
	unsigned int _seed;
};
