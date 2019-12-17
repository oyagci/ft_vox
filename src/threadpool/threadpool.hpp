#pragma once

#include <cstddef>
#include <thread>
#include <cassert>
#include <vector>
#include <future>
#include <type_traits>
#include <functional>
#include "blocking_queue.hpp"

class thread_pool
{
public:
	thread_pool(
		std::size_t queueDepth = std::thread::hardware_concurrency(),
		std::size_t threads = std::thread::hardware_concurrency())
		: _workQueue(queueDepth)
	{
		assert(queueDepth != 0);
		assert(threads != 0);

		for (std::size_t i = 0; i < threads; i++) {
			_threads.emplace_back(std::thread([this]() {
				while (true) {
					auto workItem = _workQueue.pop();
					if (workItem == nullptr) {
						_workQueue.push(nullptr);
						break ;
					}
					workItem();
				}
			}));
		}
	}

	~thread_pool() noexcept
	{
		_workQueue.push(nullptr);
		for (auto &t : _threads) {
			t.join();
		}
	}

	using Proc = std::function<void(void)>;

	template<typename F, typename... Args>
	void enqueue_work(F&& f, Args&&... args) noexcept(std::is_nothrow_invocable<decltype(&blocking_queue<Proc>::push<Proc&&>)>::value)
	{
		_workQueue.push([=]() { f(args...); });
	}

	template<typename F, typename... Args>
	auto enqueue_task(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		_workQueue.push([task]() { (*task)(); });

		return res;
	}

	bool isFull() {
		return _workQueue.size() == _workQueue.max_size();
	}

private:
	using ThreadPool = std::vector<std::thread>;
	ThreadPool _threads;
	blocking_queue<Proc> _workQueue;
};
