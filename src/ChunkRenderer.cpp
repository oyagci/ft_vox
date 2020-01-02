#include "ChunkRenderer.hpp"
#include "stb_image.h"

ChunkRenderer::ChunkRenderer() : _pool(1)
{
	_builder = std::make_unique<ChunkBuilder>();

	int width, height, nchan;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load("img/terrain.jpg", &width, &height, &nchan, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	_texture = texture;
}

ChunkRenderer::~ChunkRenderer()
{
	glDeleteTextures(1, &_texture);
}

void ChunkRenderer::addChunk(std::shared_ptr<Chunk> chunk)
{
	_chunks.push_back(chunk);
}

void ChunkRenderer::render()
{
	for (auto &m : _meshes) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);
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
