#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <memory>

using namespace lazy::graphics;

class WorldRenderer;

class Chunk
{
public:
	typedef char Block;

	enum Sides {
		TOP    = 1 << 0,
		BOTTOM = 1 << 1,
		LEFT   = 1 << 2,
		RIGHT  = 1 << 3,
		FRONT  = 1 << 4,
		BACK   = 1 << 5,
	};
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
	static constexpr std::size_t	CHUNK_SIZE = 64;


public:
	Chunk(glm::i32vec2 pos, WorldRenderer *wr);

	Block getBlock(std::size_t x, std::size_t y, std::size_t z) const;
	void setBlock(std::size_t x, std::size_t y, std::size_t z, Block val);
	const glm::vec2 &getPos() const;

	bool shouldBeRebuilt() { return _shouldBeRebuilt; }
	void setShouldBeRebuilt(bool val) { _shouldBeRebuilt = val; }

	void draw();
	void build();
	std::vector<Face> genChunkFaces();

	glm::vec3 const &getPosition() const { return _position; }
	void setPosition(glm::vec3 position) { _position = position; }

private:
	int getVisibleFaces(int x, int y, int z);
	Face genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block);

	Mesh buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces);
	void buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	glm::vec2 getTexturePosition(BlockType type);
	void markMissingNeighbors();

private:
	std::unique_ptr<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>> _blocks;
	Block _void = 0;
	glm::vec2 _worldPos;
	bool _shouldBeRebuilt;
	WorldRenderer *_worldRenderer;
	unsigned int _missingNeighbors;
	bool _isBuilt;
	Mesh _mesh;
	glm::vec3 _position;
	std::vector<Face> _faces;
};
