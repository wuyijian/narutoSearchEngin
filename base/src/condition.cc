#include "condition.h"

namespace elton
{

Condition::Condition(MutexLock & mutex)	
	: mutex_(mutex)
{
	pthread_cond_init(&mcond_, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&mcond_);
}

void Condition::notify()
{
	pthread_cond_signal(&mcond_);
}

void Condition::notifyAll()
{
	pthread_cond_broadcast(&mcond_);
}

void Condition::wait()
{
	pthread_cond_wait(&mcond_, mutex_.getMutexLockPtr());
}

}//endof namespace elton


