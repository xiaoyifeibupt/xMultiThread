#ifndef XMUTEX_H
#define XMUTEX_H

#include "xThread.h"

#include "noncopyable.h"
#include <assert.h>
#include <pthread.h>



namespace xMultiThread {

	class xMutexLock : noncopyable {
		public:
			xMutexLock()
				: holder_(0) {
				pthread_mutex_init(&mutex_, NULL);
			}

			~xMutexLock() {
				assert(holder_ == 0);
				pthread_mutex_destroy(&mutex_);
			}

			bool isLockedByThisThread() {
				return holder_ == CurrentThread::tid();
			}

			void assertLocked() {
				assert(isLockedByThisThread());
			}

			// internal usage

			void lock() {
				pthread_mutex_lock(&mutex_);
				holder_ = CurrentThread::tid();
			}

			void unlock() {
				holder_ = 0;
				pthread_mutex_unlock(&mutex_);
			}

			pthread_mutex_t* getPthreadMutex() {
				return &mutex_;
			}

		private:

			pthread_mutex_t mutex_;
			pid_t holder_;
	};

	class xMutexLockGuard : noncopyable {
		public:
			explicit xMutexLockGuard(xMutexLock& mutex) : mutex_(mutex) {
				mutex_.lock();
			}

			~xMutexLockGuard() {
				mutex_.unlock();
			}

		private:

			xMutexLock& mutex_;
	};

}

// 防止以下错误使用
// MutexLockGuard(mutex_);
// 局部对象只能锁住它所在的那一行
#define xMutexLockGuard(x) error "Missing guard object name"

#endif
