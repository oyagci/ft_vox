#include "Chunk.hpp"
#include "World.hpp"
#include "Time.hpp"

using lazy::utils::Time;

Chunk::Chunk(unsigned int seed, glm::ivec2 pos)
{
	_blocks = std::make_unique<octree::OctreeNode>(glm::vec3(pos.x, 32, pos.y), 32);
	_rawBlocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
}

auto Chunk::at(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return (_void);
	}

	return _blocks->at_local(x, y, z).value_or(_void);
}

auto Chunk::getBlock(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return (_void);
	}

	return (*_rawBlocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z];
}

void Chunk::setBlock(size_t x, size_t y, size_t z, Block val)
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return ;
	}
	(*_rawBlocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z] = val;
	_blocks->set_local(x, y, z, val);
}
