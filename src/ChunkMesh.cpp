#include "ChunkMesh.hpp"

ChunkMesh::ChunkMesh() : Mesh()
{
}
glm::vec3 const &ChunkMesh::getPosition() const
{
	return _position;
}

void ChunkMesh::setPosition(glm::vec3 position)
{
	_position = std::move(position);
}
