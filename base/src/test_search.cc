 ///
 /// @file    test_socket.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-04 18:11:26
 ///


#define MYLOG4CPP

#include "InetAddress.h"
#include "Socket.h"
#include "EpollPoller.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <memory>
#include <iostream>
#include "/home/wuyijian/simhash/src/CppJieba/QuerySegment.hpp"
#include "/home/wuyijian/simhash/src/Simhasher.hpp"

#include "mytask.h"
#include "MyLog.h"
#include "threadpool.h"
#include "myconf.h"


//创建conf对象

using namespace elton;

MyConf myconf;





    




static elton::ThreadPool * pthreadpool = NULL;

static CppJieba::KeywordExtractor * papp = NULL;

void onMessage(const elton::TcpConnectionPtr & conn);



void onConnection(const elton::TcpConnectionPtr & conn)
{
	printf("%s\n", conn->toString().c_str());
	conn->send("Hello, welcome to wd's server\n");
//	LOG_INFO("a client connected");
}


void onClose(const elton::TcpConnectionPtr & conn)
{
	printf("%s close\n", conn->toString().c_str());
}



int main(void)
{
	
	std::string str("/home/wuyijian/weibo_searchengin/bin/myconf.txt");

    myconf.loadConf(str);

    auto map_conf = myconf.getMapOfConf();

	std::cout << map_conf << std::endl; 
	
    CppJieba::KeywordExtractor app(map_conf["JIEBA_DIC_PATH"].c_str(), map_conf["JIEBA_MODEL_PATH"].c_str(),
							   map_conf["JIEBA_IDFDIC_PATH"].c_str(), map_conf["JIEBA_STOPWORDS_PATH"].c_str());

	papp = &app;

//	MyQuery * pquery = MyQuery::getInstance(app);
//	pquery->loadIndex(map_conf["MY_INDEX_PATH"]);
 //   pquery->loadPagelib(map_conf["MY_OFFSET_PATH"], map_conf["MY_LIBPAGE_PATH"]);
 //   pquery->geneJsonData();


	ThreadPool threadpool(10);
	threadpool.start(5);

	pthreadpool = &threadpool;

	InetAddress inetaddr(atoi(map_conf["MY_PORT"].c_str()));
	Socket socket;
	socket.ready(inetaddr);

	EpollPoller epollfd(socket.fd());
	epollfd.setConnectionCallback(&onConnection);
	epollfd.setMessageCallback(&onMessage);
	epollfd.setCloseCallback(&onClose);
	
	epollfd.loop();

	return 0;
}

void onMessage(const elton::TcpConnectionPtr & conn)
{
	std::string msg(conn->receive());

	MyTask task(*papp, conn, msg);
	pthreadpool->addTask(task);
}


