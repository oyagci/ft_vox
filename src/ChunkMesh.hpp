#pragma once

#include "lazy.hpp"

using lazy::graphics::Mesh;
using lazy::graphics::Camera;

class ChunkMesh : public Mesh
{
public:
	ChunkMesh();

	bool isInView(Camera &camera);
};
