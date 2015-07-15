#ifndef XCOUNTDOWNLATCH_H
#define XCOUNTDOWNLATCH_H

#include "xMutex.h"
#include "xCondition.h"

namespace xMultiThread {

	class xCountDownLatch {
		public:
			explicit xCountDownLatch(int count)
			: mutex_(),
				condition_(mutex_),
				count_(count)
				{
				}

			void wait() {
				xMutexLockGuard lock(mutex_);
				while (count_ > 0) {
					condition_.wait();
				}
			}

			void countDown() {
				xMutexLockGuard lock(mutex_);
				--count_;
				if (count_ == 0) {
					condition_.notifyAll();
				}
			}

			int getCount() const {
				xMutexLockGuard lock(mutex_);
				return count_;
			}

		private:

			xCountDownLatch(const xCountDownLatch&) = delete;
			xCountDownLatch& operator=(const xCountDownLatch&) = delete;
			
			mutable xMutexLock mutex_;
			xCondition condition_;
			int count_;
	};

}
#endif