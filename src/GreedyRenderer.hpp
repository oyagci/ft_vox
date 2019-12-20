#pragma once

#include <vector>
#include <memory>
#include "Chunk.hpp"

struct VoxelQuad
{
	glm::vec3 topLeft;
	glm::vec3 topRight;
	glm::vec3 botLeft;
	glm::vec3 botRight;
};

class GreedyRenderer
{
public:
	GreedyRenderer();
	~GreedyRenderer();

	std::vector<VoxelQuad> greedy(std::shared_ptr<Chunk> chunk);
	Mesh generateMesh(std::vector<VoxelQuad> quads);

};
