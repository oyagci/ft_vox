#include "ChunkRenderer.hpp"

ChunkRenderer::ChunkRenderer()
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
			_builder->setChunk(c);
			Mesh m = _builder->build();
			_meshes.push_back(std::move(m));
			c->setShouldBeRebuilt(false);
		}
	}
}
