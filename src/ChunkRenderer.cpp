#include "ChunkRenderer.hpp"

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

					std::vector<ChunkBuilder::Face> faces = builder.genChunkFaces();
					Mesh mesh = _builder->build(glm::vec2(c->getPos()), faces);

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
