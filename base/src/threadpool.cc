#include "threadpool.h"
//#include "mutexlock.h"
//#include "condition.h"
#include <queue>
#include <assert.h>
#include <algorithm>

//#include "mytask.h"


namespace elton
{

ThreadPool::ThreadPool(size_t que_capacity)
	: is_running_(false),
	  que_capacity_(que_capacity),
	  mutex_(),//mutex_直接构造Mutex 对象
	  notEmpty_(mutex_),//用mutex对象构造condition对象
	  notFull_(mutex_)
{}

void ThreadPool::start(size_t num)
{
	assert(threads_.empty());
	is_running_ = true;
	threads_.reserve(num);
	for(size_t i = 0; i < num; ++i)
	{
		threads_.push_back(new elton::Thread(
			std::bind(&ThreadPool::runInThread, this)));
		threads_[i]->start();
	}
}

void ThreadPool::stop()
{
	is_running_ = false;
	notEmpty_.notifyAll();
	for_each(threads_.begin(),
			 threads_.end(),
			 std::bind(&Thread::join, std::placeholders::_1));
}


ThreadPool::~ThreadPool()
{
	if(is_running_)
		this->stop();
}

void ThreadPool::addTask(MyTask task)
{
	MutexLockGuard lock(mutex_);
	while(que_task_.size() >= que_capacity_) 
	{
		notFull_.wait();
	}
//	if(!isFull())
//	{
	que_task_.push(std::move(task));
	notEmpty_.notify();
//	}
}

MyTask ThreadPool::getTask()
{
	MutexLockGuard lock(mutex_);
	while(que_task_.empty())
	{
		notEmpty_.wait();
	}
//	MyTask task;
//	if(!que_task_.empty())
//	{
	MyTask task = que_task_.front();
	que_task_.pop();
	notFull_.notify();
	return task;
//	}
}

bool ThreadPool::isEmpty()
{
	return que_task_.size() == 0;
}

bool ThreadPool::isFull()
{
	return que_task_.size() == que_capacity_;
}


void ThreadPool::runInThread()
{

	while(is_running_)
	{

		MyTask task(this->getTask());        //拷贝构造了一个task对象, while循环，让线程能够复用的关键
		task.handleQuery();
	}
}










}//end of namespace elton
