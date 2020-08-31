#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <memory>
#include "Chunk.hpp"

using namespace lazy::graphics;

class World;

class ChunkController
{
public:
	typedef int Block;

	enum class FaceDirection {
		TOP   = 1 << 0,
		BOT   = 1 << 1,
		LEFT  = 1 << 2,
		RIGHT = 1 << 3,
		FRONT = 1 << 4,
		BACK  = 1 << 5,
	};
	struct Face {
		glm::vec3 pos;
		FaceDirection dir;
		Chunk::BlockType type;
	};

	static constexpr unsigned int	TEXTURE_BLOCK_SIZE = 32;
	static constexpr unsigned int	TEXTURE_TOTAL_SIZE = 512;
	static constexpr float			TEXTURE_TILE_SIZE = static_cast<float>(TEXTURE_BLOCK_SIZE) / TEXTURE_TOTAL_SIZE;

private:
	enum class ChunkState {
		NOT_GENERATED,
		IS_GENERATING,
		NOT_BUILT,
		BUILT,
		DONE,
	};
	enum class ChunkAction {
		START_GENERATE,
		END_GENERATE,
		START_BUILD,
		END_BUILD,
		SET_DONE,
		REGENERATE,
	};

	static constexpr float			VERTICAL_OFFSET = 32.0f;
	static constexpr float			VERTICAL_OFFSET_STEP = 1.0f;

public:
	ChunkController(unsigned int seed, glm::ivec2 pos, World *world);

	Block getBlock(std::size_t x, std::size_t y, std::size_t z) const;
	void setBlock(std::size_t x, std::size_t y, std::size_t z, Block val);

	void draw();
	void update();

	glm::vec2 const &getPosition() const { return _position; }
	void setPosition(glm::vec2 position) { _position = position; }

	unsigned int getUnavailableSides();
	float getVerticalOffset() const { return _verticalOffset; }

	void onGenerate() { action(ChunkAction::START_GENERATE); }
	void onBuild() { action(ChunkAction::START_BUILD); }

private:
	void generate();
	void build();
	void action(ChunkAction action);

	void genChunkFaces();
	int getVisibleFaces(int x, int y, int z);
	Face genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block);

	Mesh buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces);
	void buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	glm::vec2 getTexturePosition(Chunk::BlockType type);

private:
	World *_world;
	Mesh _mesh;
	glm::vec2 _position;
	std::vector<Face> _faces;
	ChunkState _state;

	Chunk _data;

	float _verticalOffset;
	float _offsetTime;
};
