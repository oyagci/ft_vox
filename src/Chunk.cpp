#include "Chunk.hpp"
#include <cstdint>
#include "Cube.hpp"

Chunk::Chunk(Shader *shader)
	: _shader(shader), _pos(glm::vec3(0.0f)), _model(glm::mat4(1.0f))
{
	__builtin_memset(_blocks, 0, sizeof(_blocks));
	
	for (std::size_t i = 0; i < 100; i++) {
		__builtin_memset(_blocks + i, 1, sizeof(*_blocks));
	}
	_blocks[100][0][0] = 1;
	_blocks[100][0][2] = 1;
	
	initMesh();
}

void Chunk::onRender()
{
	std::vector<glm::vec3> positions;

	for (std::size_t height = 0; height < 100; height++) {
		for (std::size_t x = 0; x < 16; x++) {
			for (std::size_t y = 0; y < 16; y++) {
				positions.push_back(vec3(x, y, height));
			}
		}
	}

	_shader->bind();
	_shader->setUniform4x4f("modelMatrix", _model);

	_mesh.drawInstanced(positions.size(), positions);
}

void Chunk::onUpdate()
{
}

void Chunk::initMesh()
{
	vec3 vertices[] = {
		vec3(0, 0,  0), vec3(1, 0,  0), vec3(1, 1,  0), vec3(0, 1,  0),
		vec3(0, 0, -1), vec3(1, 0, -1), vec3(1, 1, -1), vec3(0, 1, -1),
	};
	vec3 triangles[] = {
		vec3(0, 1, 2), vec3(0, 2, 3), // Front
		vec3(6, 5, 4), vec3(7, 6, 4), // Back
		vec3(0, 3, 7), vec3(0, 7, 4), // Left
		vec3(1, 5, 6), vec3(1, 6, 2), // Right
		vec3(3, 2, 6), vec3(3, 6, 7), // Top
		vec3(0, 5, 1), vec3(0, 4, 5), // Bottom
	};

	_mesh = InstancedMesh(_shader);
	for (auto &vert : vertices) {
		_mesh.addPosition(std::move(vert));
	}
	for (auto &tri : triangles) {
		_mesh.addTriangle(std::move(tri));
	}
	_mesh.build();
}
