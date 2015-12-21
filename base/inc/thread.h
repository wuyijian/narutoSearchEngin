#ifndef _THREAD_H_
#define _THREAD_H_
#include <pthread.h>
#include <functional>



namespace elton
{

class Thread
{
public:
	 
	typedef std::function<void()> ThreadCallback;
	explicit Thread(ThreadCallback run_inthread);
	~Thread();
	
	void start();
	int join();
	static void * threadFunc(void * arg);

private :
	pthread_t pthread_id_;
	bool is_running_;

	ThreadCallback run_inthread_;
};

#endif

}
