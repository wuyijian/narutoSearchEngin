 ///
 /// @file    MyLog.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-10-13 09:41:25
 ///


#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <string>
#include <log4cpp/Category.hh>


namespace elton
{

enum Priority
{
	FATAL = 0,
	ERROR,
	WARN,
	INFO,
	DEBUG
};

class MyLog
{
public:
	static MyLog & getInstance();
	static void destroy();

	void fatal(const char * msg);
	void error(const char * msg);
	void warn(const char * msg);
	void info(const char * msg);
	void debug(const char * msg);

	void setPriority(Priority p);

	~MyLog();
private:
	MyLog();

private:
	static MyLog * plog_;
	log4cpp::Category & catRef_;
};

}// end of namespace elton

inline std::string int2str(int ival)
{
	std::ostringstream oss;
	oss << ival;
	return oss.str();
}


#ifdef MYLOG4CPP
	elton::MyLog & mylog = elton::MyLog::getInstance();
#else
	extern elton::MyLog &mylog;
#endif


#define postfix(msg) \
	std::string(msg).append(" (").append(__FILE__)\
		.append(":").append(__func__) \
		.append(":").append(int2str(__LINE__))\
		.append(")").c_str()

#define LOG_FATAL(msg)  mylog.fatal(postfix(msg))
#define LOG_ERROR(msg)  mylog.error(postfix(msg))
#define LOG_WARN(msg)   mylog.warn(postfix(msg))
#define LOG_INFO(msg)   mylog.info(postfix(msg))
#define LOG_DEBUG(msg)  mylog.debug(postfix(msg))

#endif
