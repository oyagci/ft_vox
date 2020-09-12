#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <future>

class ThreadPool
{
public:
	ThreadPool(size_t nThreads = std::thread::hardware_concurrency());
	~ThreadPool();

	template <typename F, typename ... Args>
	auto enqueue(F &&f, Args&& ...args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();

		{
			std::unique_lock lock(_QueueMutex);

			if (_Stop) {
				std::runtime_error("enqueue on stopped ThreadPool");
			}

			_Tasks.emplace([task]() { (*task)(); });
		}

		_Condition.notify_one();

		return res;
	}

private:
	std::vector<std::thread> _Workers;
	std::queue<std::function<void()>> _Tasks;

	std::mutex _QueueMutex;
	std::condition_variable _Condition;
	bool _Stop;
};
