#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include "thread.h"

#include <vector>
#include "noncopyable.h"
#include <queue>

#include "mutexlock.h"
#include "condition.h"

#include "mytask.h"


//class MutexLock;

//class Condition;
namespace elton
{


//class MyTask;

class ThreadPool : NonCopyable
{
public:
//	typedef std::function<void()> Task;
	explicit ThreadPool(size_t que_capacity);
	~ThreadPool();

	void start(size_t num);
	void stop();
	
	void addTask(MyTask task);

	

private:
	
//	Task thread_init_callback_;
	std::vector<Thread *> threads_;


	std::queue<MyTask> que_task_;

	void runInThread();


	bool isEmpty();
	bool isFull();

	MyTask getTask();

	MutexLock mutex_;
	Condition notEmpty_;
	Condition notFull_;

	bool is_running_;
	size_t que_capacity_;

};


}//end of namespace elton

#endif
