#include "WorldGenerator.hpp"
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "Settings.hpp"
#include "World.hpp"
#include <list>

WorldGenerator::WorldGenerator(World *world)
{
	_seed = std::any_cast<unsigned int>(Settings::instance().get("seed"));
	_factory = std::make_unique<ChunkFactory>(world, _seed);

	_stopGeneration.store(false);
	_job = std::async(std::launch::async, [this] {
		while (_stopGeneration.load() == false) {
			auto empty = [this]() -> auto { return _jobs.empty(); };
			if (!empty()) {
				glm::vec3 pos = _jobs.dequeue();
				std::shared_ptr<ChunkController> chunk = _factory->getChunk(pos);
				std::unique_lock<std::mutex> l(_cl);
				_chunks.push(chunk);
			}
		}
	});
}

WorldGenerator::~WorldGenerator()
{
	_stopGeneration.store(true);
	_job.wait();
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

	glm::ivec3 gridPos = camera.getPosition() / Chunk::CHUNK_SIZE;
	if (lastGridPos != gridPos) {
		genChunksAroundPlayer();
	}
	lastGridPos = gridPos;

	std::priority_queue<struct ChunkPriority, std::vector<struct ChunkPriority>, std::greater<struct ChunkPriority>>
		priority;
	for (auto const &c : _chunksToGenerate) {
		glm::vec3 chunkCenter(Chunk::CHUNK_SIZE / 2.0f, Chunk::CHUNK_SIZE / 2.0f, Chunk::CHUNK_SIZE / 2.0f);
		float chunkRadius = glm::length(chunkCenter);
		if (camera.sphereInFrustum(glm::vec3(c.x, 0.0f, c.z) + chunkCenter, chunkRadius)) {
			priority.push(ChunkPriority(glm::length(camera.getPosition() - c), c));
		}
		else {
			priority.push(ChunkPriority(glm::length(camera.getPosition() - c) + 128, c));
		}
	}

	if (!_chunksToGenerate.empty() && priority.size() > 0) {
		if (_jobs.size() < 4) {
			glm::vec3 chunkPos = priority.top().position;
			priority.pop();
			_chunksToGenerate.remove(chunkPos);
			_jobs.enqueue(chunkPos);
		}
	}
}

std::list<std::shared_ptr<ChunkController>> WorldGenerator::takeChunks()
{
	std::list<std::shared_ptr<ChunkController>> tmp;
	std::unique_lock<std::mutex> l(_cl);

	while (!_chunks.empty()) {
		std::shared_ptr<ChunkController> c = _chunks.front();
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
