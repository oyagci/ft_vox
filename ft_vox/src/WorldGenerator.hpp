#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <mutex>
#include "ChunkFactory.hpp"
#include <list>
#include <queue>
#include "TextRenderer.hpp"
#include <future>

class World;

class WorldGenerator
{
public:
	WorldGenerator(World *world);
	~WorldGenerator();

	void update(Camera const &camera);
	void setCameraPosition(glm::vec3 pos);
	std::list<std::shared_ptr<ChunkController>> takeChunks();
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	void genChunksAroundPlayer();

	using Priority = unsigned int;
	struct ChunkPriority {
		Priority priority;
		glm::vec3 position;
		ChunkPriority(Priority pri, glm::vec3 pos) : priority(pri), position(pos) {}
		friend bool operator<(const ChunkPriority &lhs, const ChunkPriority &rhs) {
			return lhs.priority < rhs.priority;
		}
		friend bool operator>(const ChunkPriority &lhs, const ChunkPriority &rhs) {
			return lhs.priority > rhs.priority;
		}
	};
	void addChunkToGenerate(glm::vec3 pos);

private:
	const int RENDER_DISTANCE = 6;

	std::unique_ptr<ChunkFactory> _factory;
	std::list<glm::vec3> _chunksToGenerate;

	glm::vec3 _camPos;

	std::mutex _cl;
	std::queue<std::shared_ptr<ChunkController>> _chunks;
	std::vector<glm::vec3> _generatedChunks;

	glm::ivec3 lastGridPos;
	unsigned int _seed;

	template <typename T>
	class BlockingQueue {
		public:
			void enqueue(T item)
			{
				std::unique_lock<std::mutex> l(_qLock);
				_queue.push(item);
			}

			T dequeue()
			{
				std::unique_lock<std::mutex> l(_qLock);
				T pos = _queue.front();
				_queue.pop();
				return pos;
			}

			bool empty()
			{
				std::unique_lock<std::mutex> l(_qLock);
				return _queue.size() == 0;
			}

			size_t size()
			{
				std::unique_lock<std::mutex> l(_qLock);
				return _queue.size();
			}

		private:
			std::queue<T> _queue;
			std::mutex _qLock;
	};

	std::future<void> _job;
	BlockingQueue<glm::vec3> _jobs;
	std::atomic_bool _stopGeneration;
};

