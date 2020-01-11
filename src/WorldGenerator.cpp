#include "WorldGenerator.hpp"
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "Settings.hpp"

WorldGenerator::WorldGenerator() : _pool(1)
{
	_factory = std::make_unique<ChunkFactory>();
}

glm::vec3 WorldGenerator::popPriorityChunk()
{
	glm::vec3 pos = _chunksToGenerate.top().position;
	_chunksToGenerate.pop();

	return pos;
}

void WorldGenerator::genChunksAroundPlayer()
{
	int rd = std::any_cast<int>(Settings::instance().get("renderDistance"));

	for (int xoff = -rd; xoff < rd; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z += xoff;
		for (int yoff = -rd; yoff < rd; yoff++) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * 64.0f;
			addChunkToGenerate(chunkPos, 1);
		}
	}
}

void WorldGenerator::updatePriority()
{
	std::vector<ChunkPriority> chunks;

	chunks.reserve(_chunksToGenerate.size());
	for (std::size_t i = 0; i < _chunksToGenerate.size(); i++) {
		auto e = _chunksToGenerate.top();
		chunks.push_back(e);
		_chunksToGenerate.pop();
	}

	for (auto &c : chunks) {
		c.priority = glm::length(_camPos - c.position);

		if (c.priority < 2000.0f) {
			_chunksToGenerate.push(c);
		}
	}
}

void WorldGenerator::update()
{
	genChunksAroundPlayer();
	updatePriority();
	if (!_chunksToGenerate.empty() && !_pool.isFull()) {
		glm::vec3 chunkPos  = popPriorityChunk();
		_pool.enqueue_work([=] {
				std::shared_ptr<Chunk> chunk = _factory->getChunk(std::move(chunkPos));
				std::unique_lock<std::mutex> l(_cl);
				_chunks.push(chunk);
		});
	}
}

std::vector<glm::vec3> WorldGenerator::getChunksFront()
{
	glm::i32vec3 dir = { 0, 0, -1 };
	std::vector<glm::vec3> chunks;

	for (int j = -3; j < 3; j++) {
		for (int i = 14; i >= 0; i--) {
			glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

			gridPos += (dir * i);
			gridPos *= Chunk::CHUNK_SIZE;
			gridPos += j * 64;

			chunks.push_back(glm::vec3(gridPos));
		}
	}

	return chunks;
}

void WorldGenerator::setCameraPosition(glm::vec3 pos)
{
	_camPos = std::move(pos);
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

void WorldGenerator::addChunkToGenerate(glm::vec3 pos, Priority priority)
{
	// Ignore Y axis because we only need to generate chunks on the X/Z axis.
	// If we dont do this chunks will be generated at the same X/Z coordinates for each Y unit
	// resulting in multiple times the same chunk being generated and wasting precious resources
	pos.y = 0;

	if (std::find(_generatedChunks.begin(), _generatedChunks.end(), pos) == _generatedChunks.end()) {
		_generatedChunks.push_back(glm::vec3(pos));
		_chunksToGenerate.push(ChunkPriority(priority, std::move(pos)));
	}
}
