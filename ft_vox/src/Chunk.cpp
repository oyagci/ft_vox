#include "Chunk.hpp"
#include "World.hpp"
#include "Time.hpp"

using lazy::utils::Time;

Chunk::Chunk(unsigned int seed, glm::ivec2 pos)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_position = std::move(pos);
}

auto Chunk::getBlock(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return (_void);
	}

	return (*_blocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z];
}

void Chunk::setBlock(size_t x, size_t y, size_t z, Block val)
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return ;
	}
	(*_blocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z] = val;
}
