#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>

class ChunkRenderer
{
private:
	enum FaceDirection {
		FD_TOP,
		FD_BOT,
		FD_LEFT,
		FD_RIGHT,
		FD_FRONT,
		FD_BACK,
	};
	struct Face {
		glm::vec3 pos;
		FaceDirection dir;
	};

public:
	ChunkRenderer();

	void onRender();
	void update();
	void updateChunk(Chunk &chunk);

	int getVisibleFaces(Chunk &chunk, int x, int y, int z);

	void addFaceToRender(glm::vec3 pos, FaceDirection f);
	void addBlockToRender(glm::vec3 pos);
	void buildChunkMesh(Chunk &chunk);

	void buildTopFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	void registerChunk(std::shared_ptr<Chunk> chunk);

	Chunk *getChunk(glm::u32vec2 pos);

private:
	std::vector<std::shared_ptr<Chunk>> _chunks;
	std::vector<Mesh> _meshes;
	std::vector<Face> _faces;
};
