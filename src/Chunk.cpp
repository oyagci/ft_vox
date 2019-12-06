#include "Chunk.hpp"
#include <cstdint>
#include "Cube.hpp"
#include "SimplexNoise.hpp"

Chunk::Chunk(Shader *shader)
{
	SimplexNoise sn;


	__builtin_memset(_blocks, 0, sizeof(_blocks));

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				getBlock(x, z, y) = 3 * sn.noise(x, z, y) > 1.1;
			}
		}
	}
}

Chunk::Block &Chunk::getBlock(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0 || x >= 16 || y >= 16 || z >= 16) {
		return (_void);
	}

	return _blocks[x * 16 * 16 + y * 16 + z];
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
