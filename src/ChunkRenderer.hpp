#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>

class ChunkInfo
{
public:
	ChunkInfo(std::shared_ptr<Chunk> chunk)
	{
		_chunk = chunk;
		_hasChanged = false;
	}

	void setChanged(bool changed) { _hasChanged = changed; }
	bool hasChanged() const { return _hasChanged; }
	const auto &getChunk() const { return _chunk; }

private:
	std::shared_ptr<Chunk> _chunk;
	bool _hasChanged;
};

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

private:
	std::vector<ChunkInfo> _chunks;
	std::vector<Mesh> _meshes;
	std::vector<Face> _faces;
};
