#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>

using lazy::graphics::Mesh;
using lazy::graphics::Camera;

class ChunkMesh : public Mesh
{
public:
	ChunkMesh();

	glm::vec3 const &getPosition() const;
	void setPosition(glm::vec3 position);

private:
	glm::vec3 _position;
};
