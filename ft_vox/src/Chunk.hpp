#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <memory>

using namespace lazy::graphics;

class World;

class Chunk
{
public:
	typedef int Block;

	enum class BlockType {
		AIR,
		DIRT,
		GRASS,
		GRASS_SIDE,
		STONE,
		BEDROCK,
		OAK_LOG,
		OAK_LOG_TOP,
		LEAF,
	};

	static constexpr std::size_t	CHUNK_SIZE = 64;

public:
	Chunk(unsigned int seed, glm::ivec2 pos);

	Block getBlock(std::size_t x, std::size_t y, std::size_t z) const;
	void setBlock(std::size_t x, std::size_t y, std::size_t z, Block val);

private:

	void genTrees(glm::vec3 seedOff);
	void putTree(glm::ivec3 root);

private:
	std::unique_ptr<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>> _blocks;
	Block _void = 0;
	glm::vec2 _position;

	float _verticalOffset;
	float _offsetTime;
};
