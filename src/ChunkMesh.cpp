#include "ChunkMesh.hpp"

ChunkMesh::ChunkMesh() : Mesh()
{
}

bool ChunkMesh::isInView(Camera &camera)
{
	return true;
}
