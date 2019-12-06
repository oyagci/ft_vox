#include "Chunk.hpp"
#include <cstdint>
#include "Cube.hpp"
#include "SimplexNoise.hpp"

Chunk::Chunk(glm::vec2 pos)
{
	SimplexNoise s = SimplexNoise(0.01f, 1.0f, 2.0f, 0.25f);

	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_worldPos = std::move(pos);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {
				float result = s.fractal(2,
					_worldPos.x + x, _worldPos.y + y,
					z);
				getBlock(x, y, z) = (result > 0.2f ? 1 : 0);
			}
		}
	}
}

Chunk::Block &Chunk::getBlock(std::size_t x, std::size_t y, std::size_t z)
{
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return (_void);
	}

	return (*_blocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z];
}

const glm::vec2 &Chunk::getPos() const
{
	return _worldPos;
}
