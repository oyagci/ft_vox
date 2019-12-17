#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "IRenderer.hpp"
#include "ChunkBuilder.hpp"
#include "threadpool/threadpool.hpp"
#include <queue>
#include <tuple>

class ChunkRenderer : public IRenderer
{
public:
	ChunkRenderer();

	void render() override;
	void addChunk(std::shared_ptr<Chunk> chunk);
	void update();

private:
	void buildChunks();

private:
	std::unique_ptr<ChunkBuilder> _builder;

	std::vector<Mesh> _meshes;
	std::vector<std::shared_ptr<Chunk>> _chunks;

	using Faces = std::vector<ChunkBuilder::Face>;

	std::mutex _f;
	std::queue<std::tuple<glm::vec2, Faces>> _chunkFaces;

	thread_pool _pool;
};
