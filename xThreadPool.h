#ifndef XTHREADPOOL_H
#define XTHREADPOOL_H

#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"
#include "noncopyable.h"

#include <functional>
#include <memory>

#include <vector>
#include <deque>

namespace xMultiThread {

	class ThreadPool : noncopyable {
		public:
			typedef std::function<void ()> Task;

			explicit ThreadPool(const std::string& name = std::string());
			~ThreadPool();

			void start(int numThreads);
			void stop();

			void run(const Task& f);

		private:

			void runInThread();
			Task take();

			mutable MutexLock mutex_;
			Condition cond_;
			std::string name_;
			std::vector<std::unique_ptr<xMultiThread::Thread> threads_;
			std::deque<Task> queue_;
			bool running_;
	};

}

#endif
