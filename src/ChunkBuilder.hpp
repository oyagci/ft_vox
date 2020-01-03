#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "Chunk.hpp"
#include <cassert>

class ChunkBuilder
{
public:
	enum BlockType {
		AIR,
		DIRT,
		GRASS,
		GRASS_SIDE,
		STONE,
		BEDROCK,
	};
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
		BlockType type;
	};

	static constexpr unsigned int	TEXTURE_BLOCK_SIZE = 16;
	static constexpr unsigned int	TEXTURE_TOTAL_SIZE = 256;
	static constexpr float			TEXTURE_TILE_SIZE = static_cast<float>(TEXTURE_BLOCK_SIZE) / TEXTURE_TOTAL_SIZE;

public:
	ChunkBuilder();

	const auto &getChunk() const { return _chunk; }

	void setChunk(std::shared_ptr<Chunk> chunk) { _chunk = chunk; }
	Mesh build(glm::vec2 pos, std::vector<Face> faces);

	std::vector<Face> genChunkFaces();

private:
	int getVisibleFaces(int x, int y, int z);
	Face genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block);

	Mesh buildChunkMesh(glm::vec2 chunkPos, std::vector<Face> faces);
	Mesh buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces);
	void buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	glm::vec2 getTexturePosition(BlockType type);

	std::shared_ptr<Chunk> _chunk;
};
