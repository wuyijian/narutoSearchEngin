#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace elton
{

class NonCopyable
{

protected:
	NonCopyable(){}


private:
	
	NonCopyable(const NonCopyable & rhs);          //拷贝构造

	NonCopyable & operator=(NonCopyable & rhs);    //赋值构造


	
};

}//endof namespace elton
#endif
