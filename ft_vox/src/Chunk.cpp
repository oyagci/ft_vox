#include "Chunk.hpp"
#include <cstdint>
#include "SimplexNoise.hpp"
#include <mutex>
#include "World.hpp"
#include "Time.hpp"

using lazy::utils::Time;

SimplexNoise s = SimplexNoise(0.1f, 1.0f, 2.0f, 0.25f);

float simplexNoise(size_t octaves, glm::vec3 pos)
{
	return s.fractal(octaves, pos.x, pos.y, pos.z);
}

Chunk::Chunk(unsigned int seed, glm::ivec2 pos)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_position = std::move(pos);

	glm::vec3 seedOffset(seed % 1000000, 0.0f, seed % 1000000);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = CHUNK_SIZE - 1; y + 1 > 0; y--) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

				if (y == 0) {
					setBlock(x, y, z, 4);
					continue ;
				}

				glm::vec3 pos = glm::vec3(_position.x, -32, _position.y) + glm::vec3(x, y, z) + seedOffset;

				float result = 0.0f;

				float hH = simplexNoise(1, pos / 10.0f);
				for (int i = 2; i < 3; i++) {
					hH += simplexNoise(1, pos / ((float)i * 5.0f)) * (float)i / 10.0f;
				}

				float pH = simplexNoise(1, pos / 10.0f);
				for (int i = 2; i < 6; i++) {
					pH -= simplexNoise(1, pos / (float)i) * (float)i / 16.0f;
				}

				glm::vec3 mP = glm::vec3(pos.x + 3000.0f, 0, pos.z - 400) / 50.0f;
				float mm = simplexNoise(1, mP);

				glm::vec3 dP = glm::vec3((float)pos.x - 3.4f, 827.0f, pos.z + 51.3f);
				float path = simplexNoise(1, dP);

				for (int i = 2; i < 6; i++) {
					path *= simplexNoise(1, dP / (float)i) * (float)i / 6.0f;
				}

				glm::vec3 cP = glm::vec3((float)pos.x / 2.0f - 523.432, (float)pos.y - 2.827, (float)pos.z / 2.0f + 5516.0f);
				float cave = simplexNoise(1, cP / 5.0f);
				for (int i = 0; i < 6; i++) {
					cave += simplexNoise(1, cP / ((float)i * 2.0f)) * (float)i / 6.0f;
				}

				if (abs(floor(cave * 50.0f)) - 30.0f > 0.0f) {
					cave = abs(cave * 10.0f);
				}
				else {
					cave = 0;
				}

				result = -pos.y + (pH / 2 + hH) * (CHUNK_SIZE / 2) * mm + path * 10 - cave * abs(mm);

				if (getBlock(x, y + 1, z) > 0) {
					if (getBlock(x, y + 2, z) > 0) {
						if (getBlock(x, y + 3, z) > 0) {
							setBlock(x, y, z, (y == 0 || result > 0.0f ? 2 : 0));
						}
						else {
							setBlock(x, y, z, (y == 0 || result > 0.0f ? 3 : 0));
						}
					}
					else {
						setBlock(x, y, z, (y == 0 || result > 0.0f ? 3 : 0));
					}
				}
				else {
					setBlock(x, y, z, (y == 0 || result > 0.0f ? 1 : 0));
				}
			}
		}
	}
	genTrees(seedOffset);
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

void Chunk::genTrees(glm::vec3 seedOff)
{
	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

			glm::ivec3 pos = glm::vec3(_position.x + 100, 0, _position.y + 100) + glm::vec3(x, 0, z) + seedOff;

			float n = (simplexNoise(4, glm::vec3(pos.x / 10.0f, 0.0f, pos.z / 10.0f)) + 1.0f) / 2.0f;
			float n2 = (simplexNoise(10, glm::vec3(pos.x * 10.0f, 0.0f, pos.z * 10.0f)) + 1.0f) / 2.0f;

			float val = n2 - (0.95f - n);

			if (val > 0.7f) {
				putTree(glm::vec3(x, 0, z));
			}
		}
	}
}

void Chunk::putTree(glm::ivec3 root)
{
	int treeShape[6][5][5] = {
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 6, 0, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 0, 6, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 6, 5, 6, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 5, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
		},
		{
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 5, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 5, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 5, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
	};
	for (size_t y = Chunk::CHUNK_SIZE - 4; y > 0; y--) {
		if (getBlock(root.x, y, root.z) == 1) {
			for (int yTree = 0; yTree < 6; yTree++) {
				for (int xTree = 0; xTree < 5; xTree++) {
					for (int zTree = 0; zTree < 5; zTree++) {
						if (treeShape[yTree][xTree][zTree] != 0) {
							setBlock(
								root.x + xTree - 2,
								y + 1 + (5 - yTree),
								root.z + zTree - 2,
								treeShape[yTree][xTree][zTree]);
						}
					}
				}
			}
		}
	}
}
