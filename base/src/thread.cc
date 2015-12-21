#include "thread.h"
#include <assert.h>
//#include "MyLog.h"


namespace elton
{

Thread::Thread(ThreadCallback run_inthread)
	: pthread_id_(0),
	  is_running_(false),
	  run_inthread_(run_inthread)
	{}

Thread::~Thread()
{
	if(is_running_)
		pthread_detach(pthread_id_);
}


void Thread::start()
{
	assert(!is_running_);
	if(pthread_create(&pthread_id_, NULL, threadFunc, this))
//		LOG_FATAL("pthread_create error");
	is_running_ = true;
}

int Thread::join()
{
	assert(!is_running_);
	return pthread_join(pthread_id_, NULL);
}

void * Thread::threadFunc(void * arg)
{	
	Thread * pthread = static_cast<Thread *>(arg);
	if(pthread)
	{
		pthread->run_inthread_();
		return pthread;
	}
//	delete pthread;        // thread 析构后 pthread也就delete掉了，不需要再次
							//delete
	return NULL;
}
		





















}//end of namespace elton
