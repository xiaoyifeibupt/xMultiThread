#include "../xThread.h"

#include <string>

#include <stdio.h>

#include <unistd.h>

void threadFunc() {
  printf("tid=%d\n", xMultiThread::CurrentThread::tid());
}

void threadFunc2(int x) {
  printf("tid=%d, x=%d\n", xMultiThread::CurrentThread::tid(), x);
}

void threadFunc3() {
  printf("tid=%d\n", xMultiThread::CurrentThread::tid());
  sleep(1);
}

class Foo {

public:
	explicit Foo(double x)
		: x_(x)
	{}

	void memberFunc() {
		printf("tid=%d, Foo::x_=%f\n", xMultiThread::CurrentThread::tid(), x_);
	}

	void memberFunc2(const std::string& text) {
		printf("tid=%d, Foo::x_=%f, text=%s\n", xMultiThread::CurrentThread::tid(), x_, text.c_str());
	}

private:
	double x_;
};

int main() {

	printf("pid=%d, tid=%d\n", ::getpid(), xMultiThread::CurrentThread::tid());

	xMultiThread::xThread t1(threadFunc);
	t1.start();
	t1.join();

	xMultiThread::xThread t2(std::bind(threadFunc2, 42),
				   "thread for free function with argument");
	t2.start();
	t2.join();

	Foo foo(87.53);
	xMultiThread::xThread t3(std::bind(&Foo::memberFunc, &foo),
				   "thread for member function without argument");
	t3.start();
	t3.join();

	{
		xMultiThread::xThread t5(threadFunc3);
		t5.start();
	}
	sleep(2);
}
