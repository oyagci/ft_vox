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
	double begin = glfwGetTime();

	for (auto &c : _chunks) {
		if (c->shouldBeRebuilt()) {

			if (!_pool.isFull()) {
				c->setShouldBeRebuilt(false);
				_pool.enqueue_work([=] {
					ChunkBuilder builder;
					builder.setChunk(c);
					std::unique_lock l(_cf);
					_chunkFaces.push(std::make_tuple(glm::vec2(c->getPos()), builder.genChunkFaces()));
				});
			}
		}
	}
	buildChunks();

	double end = glfwGetTime();
	std::cout << __PRETTY_FUNCTION__ << " " << (end - begin) * 1000 << " ms" << std::endl;
}

void ChunkRenderer::buildChunks()
{
	std::unique_lock l(_cf);

	if (!_chunkFaces.empty()) {
		std::tuple<glm::vec2, Faces> faces = _chunkFaces.front();
		Mesh mesh = _builder->build(std::get<0>(faces), std::get<1>(faces));
		_meshes.push_back(std::move(mesh));
		_chunkFaces.pop();
	}
}
