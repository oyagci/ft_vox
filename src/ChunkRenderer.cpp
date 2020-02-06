#include "ChunkRenderer.hpp"
#include "stb_image.h"
#include "Settings.hpp"
#include "TextureManager.hpp"
#include "World.hpp"

ChunkRenderer::ChunkRenderer(World *world) : _world(world)
{
}

ChunkRenderer::~ChunkRenderer()
{
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
void ChunkRenderer::render(Camera &camera, Shader &shader)
{
	int n = 0;
	TextureManager::instance().bind("Blocks", GL_TEXTURE0);
	for (auto &m : _chunkMap) {
		if (isInView(camera, *m.second)) {
			float offset = m.second->getVerticalOffset();
			glm::mat4 model(1.0f);

			model = glm::translate(model, glm::vec3(0.0f, offset, 0.0f));
			shader.setUniform4x4f("modelMatrix", model);
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
		if (c.second->getUnavailableSides() == 0 && c.second->shouldRegenerate()) {
			if (!_pool.isFull()) {
				c.second->onStartGeneration();
				_pool.enqueue_work([=] {
					c.second->onDoGeneration();
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
		_chunkMeshes.front()->onBuild();
		_chunkMeshes.pop();
	}
}

void ChunkRenderer::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	for (auto const &pos : chunksTooFar) {
		_chunkMap.erase(pos);
	}
}
