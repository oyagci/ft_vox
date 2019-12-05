#pragma once

#include "lazy.hpp"

using namespace lazy::graphics;

class InstancedMesh : public Mesh
{
public:
	InstancedMesh() {};
	InstancedMesh(Shader *shader) : _shader(shader) {}

	void drawInstanced(std::size_t nbInstances,
		std::vector<glm::vec3> &positions);
	void setShader(Shader *shader) { _shader = shader; }

private:
	Shader *_shader;
};
