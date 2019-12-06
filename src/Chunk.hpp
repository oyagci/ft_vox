#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include "InstancedMesh.hpp"

using namespace lazy::graphics;

class Chunk
{
public:
	typedef char Block;

public:
	Chunk(Shader *shader);

	Block &getBlock(int x, int y, int z);

private:
	Block _blocks[16 * 16 * 16];
	Block _void = 0;
};
