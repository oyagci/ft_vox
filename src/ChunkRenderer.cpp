#include "ChunkRenderer.hpp"
#include "stb_image.h"
#include "Settings.hpp"

ChunkRenderer::ChunkRenderer(WorldRenderer *worldRenderer) : _pool(1),
	_worldRenderer(worldRenderer)
{
	int width, height, nchan;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load("img/terrain.png", &width, &height, &nchan, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
	_chunkMap[chunk->getPosition()] = chunk;
}

bool ChunkRenderer::isInView(Camera &camera, Chunk &chunk)
{
	int rd = std::any_cast<int>(Settings::instance().get("renderDistance"));
	glm::vec2 pos = chunk.getPosition();
	pos.x += Chunk::CHUNK_SIZE / 2;
	pos.y += Chunk::CHUNK_SIZE / 2;

	glm::vec2 camPos2D(camera.getPosition().x, camera.getPosition().z);
	if (glm::length(camPos2D - pos) > 64.0f * (rd / 2.0f)) {
		return false;
	}

	glm::vec3 worldPos(pos.x, Chunk::CHUNK_SIZE / 2.0f, pos.y);
	float radius = glm::length(glm::vec3(Chunk::CHUNK_SIZE / 2.0f, Chunk::CHUNK_SIZE / 2.0f, 0.0f));
	return camera.sphereInFrustum(worldPos, radius);
}
void ChunkRenderer::render(Camera &camera)
{
	int n = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	for (auto &m : _chunkMap) {
		if (isInView(camera, *m.second)) {
			m.second->draw();
			n++;
		}
	}
	tr.drawText(std::to_string(n) +
		" Visible Chunks (" +
		std::to_string(_chunkMap.size()) + " total)",
		glm::vec2(10.0f, 30.0f), .3f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void ChunkRenderer::update()
{
	for (auto &c : _chunkMap) {
		c.second->update();
		if (c.second->getUnavailableSides() == 0 && c.second->shouldBeRebuilt()) {
			if (!_pool.isFull()) {
				c.second->setShouldBeRebuilt(false);
				_pool.enqueue_work([=] {
					c.second->generate();
					std::unique_lock<std::mutex> lcm(_cm);
					_chunkMeshes.push(c.second);
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
		_chunkMeshes.front()->build();
		_chunkMeshes.pop();
	}
}

void ChunkRenderer::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	for (auto const &pos : chunksTooFar) {
		_chunkMap.erase(pos);
	}
}
