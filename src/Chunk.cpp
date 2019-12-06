#include "Chunk.hpp"
#include <cstdint>
#include "Cube.hpp"
#include "SimplexNoise.hpp"

Chunk::Chunk(Shader *shader)
{
	SimplexNoise s = SimplexNoise(0.01f, 1.0f, 2.0f, 0.25f);

	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {
				float result = s.fractal(2, x, y, z);
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

//void Chunk::initMesh()
//{
//	vec3 vertices[] = {
//		vec3(0, 0,  0), vec3(1, 0,  0), vec3(1, 1,  0), vec3(0, 1,  0),
//		vec3(0, 0, -1), vec3(1, 0, -1), vec3(1, 1, -1), vec3(0, 1, -1),
//	};
//	vec3 triangles[] = {
//		vec3(0, 1, 2), vec3(0, 2, 3), // Front
//		vec3(6, 5, 4), vec3(7, 6, 4), // Back
//		vec3(0, 3, 7), vec3(0, 7, 4), // Left
//		vec3(1, 5, 6), vec3(1, 6, 2), // Right
//		vec3(3, 2, 6), vec3(3, 6, 7), // Top
//		vec3(0, 5, 1), vec3(0, 4, 5), // Bottom
//	};
//
//	_mesh = InstancedMesh(_shader);
//	for (auto &vert : vertices) {
//		_mesh.addPosition(std::move(vert));
//	}
//	for (auto &tri : triangles) {
//		_mesh.addTriangle(std::move(tri));
//	}
//	_mesh.build();
//}
