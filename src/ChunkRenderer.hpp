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
	void updateChunk(Chunk &chunk);

	int getVisibleFaces(Chunk &chunk, int x, int y, int z);

	void addBlockToRender(glm::vec3 pos);
	void buildChunkMesh(Chunk &chunk);

	void addChunk(Chunk chunk);

	// Setters
	void setShader(Shader *shader);

private:
	std::vector<Chunk> _chunks;
	std::vector<glm::vec3> _blocks;
	Shader *_shader;
	std::vector<Mesh> _meshes;
};
