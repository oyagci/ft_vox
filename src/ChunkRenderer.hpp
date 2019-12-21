#pragma once

#include <memory>
#include <queue>
#include <tuple>
#include <glm/vec3.hpp>
#include "Chunk.hpp"
#include "IRenderer.hpp"
#include "ChunkBuilder.hpp"
#include "threadpool/threadpool.hpp"
#include "ChunkMesh.hpp"

class ChunkRenderer : public IRenderer
{
public:
	ChunkRenderer();
	~ChunkRenderer();

	void render() override;
	void addChunk(std::shared_ptr<Chunk> chunk);
	void update();

private:
	void buildChunks();

private:
	std::unique_ptr<ChunkBuilder> _builder;

	std::vector<ChunkMesh> _meshes;
	std::vector<std::shared_ptr<Chunk>> _chunks;

	using Faces = std::vector<ChunkBuilder::Face>;

	std::mutex _cf;
	std::queue<std::tuple<glm::vec2, Faces>> _chunkFaces;

	thread_pool _pool;

	std::mutex _cm;
	std::queue<ChunkMesh> _chunkMeshes;

	GLuint _texture;
};
