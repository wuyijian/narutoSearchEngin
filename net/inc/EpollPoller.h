 ///
 /// @file    EpollPoller.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-06 16:12:11
 ///


#ifndef _EPOLLPOLLER_H
#define _EPOLLPOLLER_H

#include "noncopyable.h"
#include "TcpConnection.h"
#include <sys/epoll.h>
#include <vector>
#include <map>

namespace elton
{
class EpollPoller : NonCopyable
{
public:
	typedef TcpConnection::TcpConnectionCallback EpollCallback;

	EpollPoller(int listenfd);
	~EpollPoller();

	void loop();
	void unloop();

	void setConnectionCallback(EpollCallback cb);
	void setMessageCallback(EpollCallback cb);
	void setCloseCallback(EpollCallback cb);

private:
	void waitEpollfd();
	void handleConnection();
	void handleMessage(int peerfd);
	
private:
	int epollfd_;
	int listenfd_;
	bool isLooping_;

	typedef std::vector<struct epoll_event> EventList;
	EventList eventsList_;

	typedef std::map<int, TcpConnectionPtr> ConnectionMap;
	ConnectionMap connMap_;

	EpollCallback onConnectionCb_;
	EpollCallback onMessageCb_;
	EpollCallback onCloseCb_;
};


}//end of namespace wd

#endif
