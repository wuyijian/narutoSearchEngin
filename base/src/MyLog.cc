#include "MyLog.h"
#include <iostream>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>

namespace elton
{

MyLog* MyLog::plog_ = NULL;

MyLog & MyLog::getInstance()
{
	if(NULL == plog_)
	{
		plog_ = new MyLog();
	}
	return *plog_;
}

void MyLog::destroy()
{
	std::cout << "MyLog::destroy()" << std::endl;
	log4cpp::Category::shutdown();
	delete plog_;
}

MyLog::~MyLog()
{
	std::cout << "MyLog::~MyLog()" << std::endl;
}

MyLog::MyLog()
	: catRef_(log4cpp::Category::getRoot())
{
	log4cpp::PatternLayout *ptnLy1 = new log4cpp::PatternLayout();
	ptnLy1->setConversionPattern("%d:%p %c %x:%m%n");
	log4cpp::PatternLayout *ptnLy2 = new log4cpp::PatternLayout();
	ptnLy2->setConversionPattern("%d:%p %c %x:%m%n");

	log4cpp::OstreamAppender *oStreamAppender = 
		new log4cpp::OstreamAppender("oStreamAppender", &std::cout);
	oStreamAppender->setLayout(ptnLy1);
		
	log4cpp::FileAppender * fileAppender = 
		new log4cpp::FileAppender("fileAppender", "elton.log");
	fileAppender->setLayout(ptnLy2);

	catRef_.addAppender(oStreamAppender);
	catRef_.addAppender(fileAppender);

	catRef_.setPriority(log4cpp::Priority::WARN);
	
	std::cout << "MyLog::MyLog()" << std::endl;
}

void MyLog::setPriority(Priority p)
{
	switch(p)
	{
	case FATAL:
		catRef_.setPriority(log4cpp::Priority::FATAL);
		break;
	case ERROR:
		catRef_.setPriority(log4cpp::Priority::ERROR);
		break;
	case WARN:
		catRef_.setPriority(log4cpp::Priority::WARN);
		break;
	case INFO:
		catRef_.setPriority(log4cpp::Priority::INFO);
		break;
	case DEBUG:
		catRef_.setPriority(log4cpp::Priority::DEBUG);
		break;
	default:
		catRef_.setPriority(log4cpp::Priority::DEBUG);
	}
}

void MyLog::fatal(const char * msg)
{
	//std::cout << "MyLog::fatal()" << std::endl;
	catRef_.fatal(msg);
}

void MyLog::error(const char * msg)
{
	//std::cout << "Mylog::error()" << std::endl;
	catRef_.error(msg);
}

void MyLog::warn(const char * msg)
{
	//std::cout << "Mylog::warn()" << std::endl;
	catRef_.warn(msg);
}

void MyLog::info(const char * msg)
{
	//std::cout << "Mylog::info()" << std::endl;
	catRef_.info(msg);
}

void MyLog::debug(const char * msg)
{
	//std::cout << "Mylog::debug()" << std::endl;
	catRef_.debug(msg);
}

}// end of namespace elton
