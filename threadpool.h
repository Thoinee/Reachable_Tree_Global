#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>

class ThreadPool
{
public:
	ThreadPool() :ThreadPool(0) {};
	ThreadPool(int n) :num_threads(n) {
		num_no_sleeping = num_threads;
	};
	void run() {
		threads.resize(num_threads);
		for (int i = 0; i < num_threads; ++i) {
			threads[i] = std::thread(&ThreadPool::task, this);
		}
		for (int i = 0; i < num_threads; ++i) {
			if (threads[i].joinable())
				threads[i].join();
		}
	}
	void stop() {}
	virtual void task() {};
	virtual ~ThreadPool() {
		for (int i = 0; i < threads.size(); ++i) {
			if (threads[i].joinable())
				threads[i].join();
		}
	};
public:
	int num_no_sleeping;
	int num_threads;
	std::vector<std::thread> threads;
};