#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include "InstancedMesh.hpp"

using namespace lazy::graphics;

class Chunk
{
public:
	Chunk(Shader *shader);

	void onRender();
	void onUpdate();

private:
	void initMesh();

private:
	char _blocks[256][16][16];
	Shader *_shader;
	glm::vec3 _pos;
	glm::mat4 _model;
	InstancedMesh _mesh;
};
