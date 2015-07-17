#ifndef XTHREAD_H
#define XTHREAD_H

#include "noncopyable.h"

#include <pthread.h>
#include <functional>
#include <memory>

namespace xMultiThread {

	class xThread : noncopyable {
		public:
			typedef std::function<void ()> ThreadFunc;

			explicit xThread(const ThreadFunc&, const std::string& name = std::string());
			
			~xThread();

			void start();
			void join();

			bool started() const { return started_; }

			pid_t tid() const { return *tid_; }
			
			const std::string& name() const { return name_; }

		private:

			bool        started_;
			bool        joined_;
			pthread_t   pthreadId_;
			std::shared_ptr<pid_t> tid_;
			ThreadFunc  func_;
			std::string name_;
	};

	namespace CurrentThread {
		pid_t tid();
		const char* name();
		bool isMainThread();
	}

}

#endif