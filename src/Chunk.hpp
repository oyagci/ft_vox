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

	static constexpr std::size_t CHUNK_SIZE = 128;

public:
	Chunk(Shader *shader);

	Block &getBlock(std::size_t x, std::size_t y, std::size_t z);

private:
	Block _blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	Block _void = 0;
};
