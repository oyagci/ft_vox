#pragma once

#include "ChunkController.hpp"

class World;

class ChunkFactory
{
public:
	ChunkFactory(World *world, unsigned int seed);
	std::unique_ptr<ChunkController> getChunk(glm::vec3 pos);
private:
	World *_world;
	unsigned int _seed;
};
