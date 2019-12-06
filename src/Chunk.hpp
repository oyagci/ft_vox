#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include "InstancedMesh.hpp"
#include <memory>

using namespace lazy::graphics;

class Chunk
{
public:
	typedef char Block;

	static constexpr std::size_t CHUNK_SIZE = 64;

public:
	Chunk(Shader *shader);

	Block &getBlock(std::size_t x, std::size_t y, std::size_t z);

private:
	std::unique_ptr<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>> _blocks;
	Block _void = 0;
};
