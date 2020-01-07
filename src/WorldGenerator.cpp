#include "WorldGenerator.hpp"
#include "Chunk.hpp"
#include <glm/vec3.hpp>

WorldGenerator::WorldGenerator() : _pool(1)
{
	_factory = std::make_unique<ChunkFactory>();
	genChunksAroundPlayer();
}

glm::vec3 WorldGenerator::popPriorityChunk()
{
	glm::vec3 pos = _chunksToGenerate.top().position;
	_chunksToGenerate.pop();

	return pos;
}

void WorldGenerator::genChunksAroundPlayer()
{
	std::vector<glm::vec3> chunks = getChunksAround();
	for (auto &c : chunks) {
		addChunkToGenerate(std::move(c), 0);
	}

	chunks = getChunksTriangleNorth();
	for (auto &c : chunks) {
		addChunkToGenerate(std::move(c), 1);
	}

	chunks = getChunksTriangleSouth();
	for (auto &c : chunks) {
		addChunkToGenerate(std::move(c), 4);
	}

	chunks = getChunksTriangleEast();
	for (auto &c : chunks) {
		addChunkToGenerate(std::move(c), 2);
	}

	chunks = getChunksTriangleWest();
	for (auto &c : chunks) {
		addChunkToGenerate(std::move(c), 3);
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
		_chunksToGenerate.push(c);
	}
}

void WorldGenerator::update(glm::vec3 camPos)
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

std::vector<glm::vec3> WorldGenerator::getChunksAround()
{
	std::vector<glm::vec3> chunks;
	glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

	std::array<glm::i32vec3, 9> positions = {
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z    ), // Center
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z    ), // Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z    ), // Right
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z + 1), // Front
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z - 1), // Back
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z + 1), // Front Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z + 1), // Front Right
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z - 1), // Back Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z - 1), // Back Right
	};

	for (auto &p : positions) {
		glm::vec3 finalPos = glm::vec3(p) * Chunk::CHUNK_SIZE;
		chunks.push_back(glm::vec3(finalPos));
	}

	return chunks;
}

std::vector<glm::vec3> WorldGenerator::getChunksTriangleNorth()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z += xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldGenerator::getChunksTriangleSouth()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z -= xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldGenerator::getChunksTriangleEast()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.x += xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x, gridPos.y, gridPos.z + yoff) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldGenerator::getChunksTriangleWest()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.x -= xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x, gridPos.y, gridPos.z + yoff) * 64.0f;
			chunks.push_back(std::move(chunkPos));
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
	if (std::find(_generatedChunks.begin(), _generatedChunks.end(), pos) == _generatedChunks.end()) {
		_generatedChunks.push_back(glm::vec3(pos));
		_chunksToGenerate.push(ChunkPriority(priority, std::move(pos)));
	}
}
