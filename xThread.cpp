#include "xThread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <assert.h>

namespace xMultiThread {

	namespace CurrentThread {
		__thread const char* t_threadName = "unknown";
	}
}

namespace {

	__thread pid_t t_cachedTid = 0;

	pid_t gettid() {
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	void afterFork() {
		t_cachedTid = gettid();
		xMultiThread::CurrentThread::t_threadName = "main";
	}

	class ThreadNameInitializer {
	
		public:
			ThreadNameInitializer() {
				xMultiThread::CurrentThread::t_threadName = "main";
				pthread_atfork(NULL, NULL, &afterFork);
			}
	};

	ThreadNameInitializer init;

	struct ThreadData {
		typedef xMultiThread::xThread::ThreadFunc ThreadFunc;
		ThreadFunc func_;
		std::string name_;
		std::weak_ptr<pid_t> wkTid_;

		ThreadData(const ThreadFunc& func,
					const std::string& name,
					const std::shared_ptr<pid_t>& tid)
				: func_(func),
				name_(name),
				wkTid_(tid)
				{}

		void runInThread() {
			pid_t tid = xMultiThread::CurrentThread::tid();
			std::shared_ptr<pid_t> ptid = wkTid_.lock();

			if (ptid) {
				*ptid = tid;
				ptid.reset();
			}

			xMultiThread::CurrentThread::t_threadName = name_.c_str();
			func_(); // FIXME: surround with try-catch, see xMultiThread
			xMultiThread::CurrentThread::t_threadName = "finished";
		}
	};

	void* startThread(void* obj) {
		ThreadData* data = static_cast<ThreadData*>(obj);
		data->runInThread();
		delete data;
		return NULL;
	}

}

using namespace xMultiThread;

pid_t CurrentThread::tid() {
	if (t_cachedTid == 0) {
		t_cachedTid = gettid();
 	}
	return t_cachedTid;
}

const char* CurrentThread::name() {
	return t_threadName;
}

bool CurrentThread::isMainThread() {
	return tid() == ::getpid();
}

xThread::xThread(const ThreadFunc& func, const std::string& n)
	: started_(false),
		joined_(false),
		pthreadId_(0),
		tid_(new pid_t(0)),
		func_(func),
		name_(n)
	{}

xThread::~xThread() {
  if (started_ && !joined_) {
		pthread_detach(pthreadId_);
	}
}

void xThread::start() {
	assert(!started_);
	started_ = true;

	ThreadData* data = new ThreadData(func_, name_, tid_);
	if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
		started_ = false;
		delete data;
		abort();
	}
}

void xThread::join() {
	assert(started_);
	assert(!joined_);
	joined_ = true;
	pthread_join(pthreadId_, NULL);
}
