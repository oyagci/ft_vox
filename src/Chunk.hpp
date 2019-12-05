#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include "InstancedMesh.hpp"

using namespace lazy::graphics;

class Chunk
{
public:
	typedef unsigned char Block;
	typedef std::array<std::array<std::array<Block, 16>, 16>, 256> Blocks; // [Height][X][Y]

public:
	Chunk(Shader *shader);

	void onUpdate();

	// Getters
	const Blocks &getBlocks() const { return _blocks; };
	const Block getBlockAt(std::size_t x, std::size_t y, std::size_t z) const
	{ 
		return _blocks[x][y][z];
	}

private:
	Blocks _blocks;
	glm::vec3 _pos;
};
