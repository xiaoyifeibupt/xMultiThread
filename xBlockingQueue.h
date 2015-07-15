#ifndef XBLOCKINGQUEUE_H
#define XBLOCKINGQUEUE_H

#include "xCondition.h"
#include "xMutex.h"

#include <deque>
#include <assert.h>

namespace xMultiThread {

template<typename T>
class xBlockingQueue {
	public:
		xBlockingQueue()
			: mutex_(),
			notEmpty_(mutex_),
			queue_()
			{
			}

		void put(const T& x) {
			xMutexLockGuard lock(mutex_);
			queue_.push_back(x);
			notEmpty_.notify();
  		}

		T take() {
			xMutexLockGuard lock(mutex_);
			// always use a while-loop, due to spurious wakeup
			while (queue_.empty()) {
				notEmpty_.wait();
			}
			assert(!queue_.empty());
			T front(queue_.front());
			queue_.pop_front();
			return front;
		}

		size_t size() const {
			xMutexLockGuard lock(mutex_);
			return queue_.size();
		}

	private:

		xBlockingQueue(const xBlockingQueue&) = delete;
		xBlockingQueue& operator=(const xBlockingQueue&) = delete;

		mutable xMutexLock mutex_;
		xCondition         notEmpty_;
		std::deque<T>     queue_;
	};

}

#endif