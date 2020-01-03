#include "Chunk.hpp"
#include <cstdint>
#include "SimplexNoise.hpp"

SimplexNoise s = SimplexNoise(0.1f, 1.0f, 2.0f, 0.25f);

float simplexNoise(size_t octaves, glm::vec3 pos)
{
	return s.fractal(octaves, pos.x, pos.y, pos.z);
}

Chunk::Chunk(glm::i32vec2 pos) : _shouldBeRebuilt(true)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_worldPos = std::move(pos);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = CHUNK_SIZE - 1; y + 1 > 0; y--) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

				if (y == 0) {
					getBlock(x, y, z) = 4;
					continue ;
				}

				glm::vec3 pos = glm::vec3(_worldPos.x, -32, _worldPos.y) + glm::vec3(x, y, z);
				float result = 0.0f;

				float hH = simplexNoise(1, pos / 10);
				for (int i = 2; i < 6; i++) {
					hH += simplexNoise(1, pos / (i * 5)) * i / 16;
				}

				float pH = simplexNoise(1, pos / 10);
				for (int i = 2; i < 6; i++) {
					pH -= simplexNoise(1, pos / i) * i / 16;
				}

				glm::vec3 mP = glm::vec3(pos.x + 3000.0f, 0, pos.z -400) / 50;
				float mm = simplexNoise(1, mP);

				glm::vec3 dP = glm::vec3(pos.x - 3.4f, 827, z + 51.3f);
				float path = simplexNoise(1, dP);

				for (int i = 2; i < 6; i++) {
					path *= simplexNoise(1, dP / i) * i / 6;
				}

				glm::vec3 cP = glm::vec3(pos.x / 2 - 523.432, pos.y - 2.827, z / 2 + 5516.0f);
				float cave = simplexNoise(1, cP / 5);
				for (int i = 0; i < 6; i++) {
					cave += simplexNoise(1, cP / (i * 2)) * i / 6;
				}

				if (abs(floor(cave * 50)) - 30 > 0) {
					cave = abs(cave * 10);
				}
				else {
					cave = 0;
				}

				result = -pos.y + (pH / 2 + hH) * (CHUNK_SIZE / 2) * mm + path * 10 - cave * abs(mm);

				if (getBlock(x, y + 1, z) > 0) {
					if (getBlock(x, y + 2, z) > 0) {
						if (getBlock(x, y + 3, z) > 0) {
							getBlock(x, y, z) = (y == 0 || result > 0.0f ? 2 : 0);
						}
						else {
							getBlock(x, y, z) = (y == 0 || result > 0.0f ? 3 : 0);
						}
					}
					else {
						getBlock(x, y, z) = (y == 0 || result > 0.0f ? 3 : 0);
					}
				}
				else {
					getBlock(x, y, z) = (y == 0 || result > 0.0f ? 1 : 0);
				}
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
