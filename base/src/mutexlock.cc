#include "mutexlock.h"

namespace elton
{


MutexLock::MutexLock()
{
	pthread_mutex_init(&mlock_, NULL);
}


MutexLock::~MutexLock()
{
	pthread_mutex_destroy(&mlock_);
}	

void MutexLock::lock()
{
	pthread_mutex_lock(&mlock_);
}

void MutexLock::unlock()
{
	pthread_mutex_unlock(&mlock_);
}

pthread_mutex_t * MutexLock::getMutexLockPtr()
{
	return & mlock_;
}

MutexLockGuard::MutexLockGuard(MutexLock & mutex)
	: mutex_(mutex)
{
	mutex_.lock();
}

MutexLockGuard::~MutexLockGuard()
{
	mutex_.unlock();
}

}//end of namespace elton







