#ifndef _MUTEXLOCK_H_
#define _MUTEXLOCK_H_
#include <pthread.h>
#include "noncopyable.h"



namespace elton
{

class MutexLock : NonCopyable
{

public:
	MutexLock();
	
	~MutexLock();

	void lock();

	void unlock();

	pthread_mutex_t * getMutexLockPtr();
	
private:

	pthread_mutex_t mlock_;

};



class MutexLockGuard
{
public:
	MutexLockGuard(MutexLock & mutex);
	~MutexLockGuard();

private:
	MutexLock & mutex_;

};



}//end of namespace elton

#endif

