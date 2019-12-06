#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>

class ChunkRenderer
{
public:
	ChunkRenderer();

	void onRender();
	void update();

	int getVisibleFaces(int x, int y, int z);

	void addBlockToRender(glm::vec3 pos);

	// Setters
	void setChunk(Chunk chunk);
	void setShader(Shader *shader);

private:
	Chunk _chunk;
	std::vector<glm::vec3> _blocks;
	Shader *_shader;
	Mesh _mesh;
};
