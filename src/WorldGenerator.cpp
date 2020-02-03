#include "WorldGenerator.hpp"
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "Settings.hpp"
#include "WorldRenderer.hpp"
#include <list>

WorldGenerator::WorldGenerator(WorldRenderer *wr) : _pool(1)
{
	_factory = std::make_unique<ChunkFactory>(wr);
}

void WorldGenerator::genChunksAroundPlayer()
{
	int rd = std::any_cast<int>(Settings::instance().get("renderDistance"));

	for (int xoff = -(rd / 2); xoff < (rd / 2); xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z += xoff;
		for (int yoff = -(rd / 2); yoff < (rd / 2); yoff++) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * Chunk::CHUNK_SIZE;
			addChunkToGenerate(chunkPos);
		}
	}
}


void WorldGenerator::update(Camera const &camera)
{
	_camPos = camera.getPosition();

	glm::vec3 gridPos = camera.getPosition() / Chunk::CHUNK_SIZE;
	if (lastGridPos != gridPos) {
		genChunksAroundPlayer();
	}
	lastGridPos = gridPos;

	std::priority_queue<ChunkPriority, std::vector<ChunkPriority>, std::greater<ChunkPriority>> priority;
	for (auto const &c : _chunksToGenerate) {
		if (camera.sphereInFrustum(glm::vec3(c.x, 32.0f, c.z),
				glm::length(glm::vec3(64.0f, 64.0f, 0.0f)))) {
			priority.push(ChunkPriority(glm::length(camera.getPosition() - c), c));
		}
	}

	if (!_chunksToGenerate.empty() && !_pool.isFull()) {
		if (priority.size() > 0) {
			glm::vec3 chunkPos = priority.top().position;
			priority.pop();
			_chunksToGenerate.remove(chunkPos);
			_pool.enqueue_work([=] {
					std::shared_ptr<Chunk> chunk = _factory->getChunk(chunkPos);
					std::unique_lock<std::mutex> l(_cl);
					_chunks.push(chunk);
			});
		}
	}
}

std::list<std::shared_ptr<Chunk>> WorldGenerator::takeChunks()
{
	std::list<std::shared_ptr<Chunk>> tmp;

	while (!_chunks.empty()) {
		std::shared_ptr<Chunk> c = _chunks.front();
		tmp.push_front(c);
		_chunks.pop();
	}

	return tmp;
}

void WorldGenerator::addChunkToGenerate(glm::vec3 pos)
{
	// Ignore Y axis because we only need to generate chunks on the X/Z axis.
	// If we dont do this chunks will be generated at the same X/Z coordinates for each Y unit
	// resulting in multiple times the same chunk being generated and wasting precious resources
	pos.y = 0;

	if (std::find(_generatedChunks.begin(), _generatedChunks.end(), pos) == _generatedChunks.end()) {
		_generatedChunks.push_back(glm::vec3(pos));
		_chunksToGenerate.push_back(std::move(pos));
	}
}

void WorldGenerator::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	std::vector<glm::vec3> chunks;
	int n = 0;

	for (auto &c : _generatedChunks) {
		bool remove = false;
		for (auto const &pos : chunksTooFar) {
			if (pos.x == c.x && pos.y == c.z) {
				remove = true;
				n++;
			}
		}
		if (!remove) {
			chunks.push_back(c);
		}
	}

	_generatedChunks = chunks;
}
