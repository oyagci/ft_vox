#include "Chunk.hpp"
#include <cstdint>
#include "Cube.hpp"
#include "SimplexNoise.hpp"

SimplexNoise s = SimplexNoise(0.06f, 1.0f, 2.0f, 0.25f);

Chunk::Chunk(glm::i32vec2 pos)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_worldPos = std::move(pos);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

				glm::vec3 pos = glm::vec3(_worldPos.x, -32, _worldPos.y) + glm::vec3(x, y, z);
				float result = 0.0f;

				float hH = s.fractal(1, pos.x / 10, pos.y / 10, pos.z / 10);
				for (int i = 2; i < 6; i++) {
					hH += s.fractal(1, pos.x / (i * 5), pos.y / (i * 5), pos.z / (i * 5)) * (i / 6);
				}

				float pH = s.fractal(1, pos.x / 10, pos.y / 10, pos.z / 10);
				for (int i = 2; i < 6; i++) {
					pH -= s.fractal(1, pos.x / i, pos.y / i, pos.z / i) * (i / 6);
				}

				glm::vec3 mP = glm::vec3(pos.x + 3000.0f, 0, pos.z -400) / 50;
				float mm = s.fractal(1, mP.x, mP.y, mP.z);

				glm::vec3 dP = glm::vec3(pos.x - 3.4f, 827, z + 51.3f);
				float path = s.fractal(1, dP.x, dP.y, dP.z);

				for (int i = 2; i < 6; i++) {
					path *= s.fractal(1, dP.x / i, dP.y / i, dP.z / i) * (i / 6);
				}

				result = -pos.y + (pH / 2 + hH) * (CHUNK_SIZE/2) * mm + path * 10;

				getBlock(x, y, z) = (y == 0 || result > 0.0f ? 1 : 0);
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
