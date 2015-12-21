#ifndef MY_TASK_H
#define MY_TASK_H

#include <string>
#include "/home/wuyijian/simhash/src/CppJieba/QuerySegment.hpp"
#include "/home/wuyijian/simhash/src/Simhasher.hpp"
#include "TcpConnection.h"


namespace elton
{
class MyTask
{
public:

	MyTask(const CppJieba::KeywordExtractor & app,
			const TcpConnectionPtr & conn, const std::string & search_words);

	void handleQuery();

	void handleCatch();

private:
	
	const CppJieba::KeywordExtractor & app_;

	std::string json_data_;

	const std::string search_words_;

	const TcpConnectionPtr & conn_;

	std::map<std::string, std::set<std::pair<uint64_t, double> > > map_index_;

};
}//endof elton??


#endif
