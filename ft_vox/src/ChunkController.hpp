#pragma once

#include "lazy.hpp"
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <memory>
#include "Chunk.hpp"
#include <optional>
#include "Time.hpp"
#include <atomic>
#include "threadpool/ThreadPool.hpp"

using namespace lazy::graphics;

class World;

class ChunkControllerState;
class ChunkControllerState_NotGenerated;
class ChunkControllerState_IsGenerating;
class ChunkControllerState_NotBuilt;

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

private:
	void generate(size_t step);
	void build();

	void genChunkData();
	void genChunkFaces(size_t step);
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

	void genTrees(glm::vec3 seedOff);
	void putTree(glm::ivec3 root);

private:
	World *_world;
	Mesh _mesh;
	glm::vec2 _position;
	std::vector<Face> _faces;
	bool _isGenerated;
	unsigned int _seed;

	std::unique_ptr<ChunkControllerState> _state;

	Chunk _data;

	float _verticalOffset;
	float _offsetTime;

	friend class ChunkControllerState;
	friend class ChunkControllerState_NotGenerated;
	friend class ChunkControllerState_IsGenerating;
	friend class ChunkControllerState_NotBuilt;
	friend class ChunkControllerState_Built;
};

class ChunkControllerState
{
public:
	ChunkControllerState(ChunkController *controller)
		: _Controller(controller) {}
	virtual ~ChunkControllerState() {}

	virtual std::optional<ChunkControllerState*> OnRegenerate() { return std::nullopt; }
	virtual std::optional<ChunkControllerState*> OnDraw()       { return std::nullopt; }
	virtual std::optional<ChunkControllerState*> OnUpdate()     { return std::nullopt; }

protected:
	ChunkController *_Controller = nullptr;
};

// Chunk Controller State
// ======================

class ChunkControllerState_NotGenerated : public ChunkControllerState
{
private:
	size_t _Step = 0;

public:
	ChunkControllerState_NotGenerated(ChunkController *controller)
		: ChunkControllerState(controller) {}

	std::optional<ChunkControllerState*> OnUpdate() override;
};


class ChunkControllerState_IsGenerating : public ChunkControllerState
{
public:
	ChunkControllerState_IsGenerating(ChunkController *controller)
		: ChunkControllerState(controller) {}

	std::optional<ChunkControllerState*> OnUpdate() override;
};


class ChunkControllerState_NotBuilt : public ChunkControllerState
{
public:
	ChunkControllerState_NotBuilt(ChunkController *controller)
		: ChunkControllerState(controller) {}

	std::optional<ChunkControllerState*> OnUpdate() override;
};


class ChunkControllerState_Built : public ChunkControllerState
{
public:
	ChunkControllerState_Built(ChunkController *controller)
		: ChunkControllerState(controller) {}

	std::optional<ChunkControllerState*> OnDraw() override;
	std::optional<ChunkControllerState*> OnUpdate() override;
};

class ChunkControllerState_Done : public ChunkControllerState
{
public:
	ChunkControllerState_Done(ChunkController *controller)
		: ChunkControllerState(controller) {}

	std::optional<ChunkControllerState*> OnRegenerate() override;
	std::optional<ChunkControllerState*> OnDraw() override;
};
