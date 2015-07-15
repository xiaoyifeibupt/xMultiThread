#ifndef XCONDITION_H
#define XCONDITION_H

#include "xMutex.h"

#include <pthread.h>
#include <errno.h>

namespace xMultiThread {

	class xCondition {
	public:
		explicit xCondition(xMutexLock& mutex) : mutex_(mutex) {
			pthread_cond_init(&pcond_, NULL);
		}

	  ~xCondition() {
			pthread_cond_destroy(&pcond_);
		}

		void wait() {
			pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
		}

		// returns true if time out, false otherwise.
		bool waitForSeconds(int seconds) {
			struct timespec abstime;
			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += seconds;
			return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
		}

		void notify() {
			pthread_cond_signal(&pcond_);
		}

		void notifyAll() {
			pthread_cond_broadcast(&pcond_);
		}

	private:

		xCondition(const xCondition&) = delete;
		xCondition& operator=(const xCondition&) = delete;

		xMutexLock& mutex_;
		pthread_cond_t pcond_;
	};
}
#endif
