#pragma once

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t nThreads) : _Stop(false)
{
	for (size_t i = 0; i < nThreads; i++) {
		_Workers.emplace_back(

			[this] {

				std::function<void()> task;

				while (true) {
					{
						std::unique_lock lock(_QueueMutex);

						// Wait until we either need to stop the worker or there is a task to complete
						_Condition.wait(lock, [this] { return _Stop || !_Tasks.empty(); });

						// Stop only if there is no work left
						if (_Stop && _Tasks.empty()) {
							return;
						}

						// Remove the task from the task queue
						task = std::move(_Tasks.front());
						_Tasks.pop();
					} // Release the lock

					// Execute the task
					task();
				}
			}
		);
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock lock(_QueueMutex);
		_Stop = true;
	}
	// Wake up all the workers so they can stop
	_Condition.notify_all();

	for (auto &w : _Workers) {
		w.join();
	}
}
