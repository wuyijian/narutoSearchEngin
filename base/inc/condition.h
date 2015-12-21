#ifndef _CONDITION_H_
#define _CONDITION_H_
#include "mutexlock.h"
#include "noncopyable.h"

namespace elton
{

class Condition : NonCopyable
{
public:
	explicit Condition(MutexLock & mutex);
	~Condition();

	void notify();
	void notifyAll();

	void wait();

private:
	
	pthread_cond_t mcond_;
	MutexLock & mutex_;
};


}//endof namespace elton


#endif
