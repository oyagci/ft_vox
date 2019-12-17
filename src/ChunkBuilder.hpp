#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "Chunk.hpp"
#include <cassert>

class ChunkBuilder
{
public:
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
	ChunkBuilder();

	const auto &getChunk() const { return _chunk; }

	void setChunk(std::shared_ptr<Chunk> chunk) { _chunk = chunk; }
	Mesh build(glm::vec2 pos, std::vector<Face> faces);

	std::vector<Face> genChunkFaces();

private:
	int getVisibleFaces(int x, int y, int z);
	Face genFaceToRender(glm::vec3 pos, FaceDirection f);

	Mesh buildChunkMesh(glm::vec2 chunkPos, std::vector<Face> faces);
	Mesh buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces);
	void buildTopFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	std::shared_ptr<Chunk> _chunk;
};
