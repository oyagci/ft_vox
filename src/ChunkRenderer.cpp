#include "ChunkRenderer.hpp"
#include "GreedyRenderer.hpp"

ChunkRenderer::ChunkRenderer() : _pool(1)
{
	_builder = std::make_unique<ChunkBuilder>();
}

void ChunkRenderer::addChunk(std::shared_ptr<Chunk> chunk)
{
	_chunks.push_back(chunk);
}

void ChunkRenderer::render()
{
	for (auto &m : _meshes) {
		m.draw();
	}
}

void ChunkRenderer::update()
{
	for (auto &c : _chunks) {
		if (c->shouldBeRebuilt()) {
			if (!_pool.isFull()) {
				c->setShouldBeRebuilt(false);
				_pool.enqueue_work([=] {
					ChunkBuilder builder;
					builder.setChunk(c);

					GreedyRenderer grdy;
					std::vector<VoxelQuad> vq = grdy.greedy(c);
					Mesh mesh = grdy.generateMesh(std::move(vq));

					std::unique_lock<std::mutex> lcm(_cm);
					_chunkMeshes.push(ChunkMesh(mesh));
				});
			}
		}
	}
	buildChunks();
}

void ChunkRenderer::buildChunks()
{
	std::unique_lock lm(_cm);
	if (!_chunkMeshes.empty()) {
		ChunkMesh cm = _chunkMeshes.front();
		if (!cm.isBuilt) {
			cm.build();
		}
		_meshes.push_back(cm);
		_chunkMeshes.pop();
	}
}
