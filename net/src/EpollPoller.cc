 //
 /// @file    EpollPoller.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-06 16:18:29
 ///

#include "EpollPoller.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>


namespace elton
{

int createEpollFd()
{
	int efd = ::epoll_create1(0);
	if(-1 == efd)
	{
		perror("epoll_create1 error");
		exit(EXIT_FAILURE);
	}
	return efd;
}

void addEpollFdRead(int efd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;
	int ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
	if(-1 == ret)
	{
		perror("epoll_ctl add error");
		exit(EXIT_FAILURE);
	}
}

void delEpollReadFd(int efd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	int ret = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev);
	if(-1 == ret)
	{
		perror("epoll_ctl del error");
		exit(EXIT_FAILURE);
	}
}

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts < 0)
	{
		perror("fcntl(sock, F_GETFL)");
		exit(-1);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0)
	{
		perror("fcntl(sock, F_SETFL, opts)");
		exit(-1);
	}
}

int acceptConnFd(int listenfd)
{
	int peerfd = ::accept(listenfd, NULL, NULL);
		
	setnonblocking(peerfd);	

	if(peerfd == -1)
	{
		perror("accept error");
		exit(EXIT_FAILURE);
	}
	return peerfd;
}


//预览数据
size_t recvPeek(int sockfd, void * buf, size_t len)
{
	int nread;
	do
	{
		nread = ::recv(sockfd, buf, len, MSG_PEEK);
	}while(nread == -1 && errno == EINTR);
	return nread;
}

//通过预览数据，判断conn是否关闭
bool isConnectionClosed(int sockfd)
{
	char buf[1024];
	int nread = recvPeek(sockfd, buf, sizeof(buf));
	if(-1 == nread)
	{
		perror("recvPeek");
		exit(EXIT_FAILURE);
	}
	return (0 == nread);
}

//==========================================


EpollPoller::EpollPoller(int listenfd)
	: epollfd_(createEpollFd()),
	  listenfd_(listenfd),
	  isLooping_(false),
	  eventsList_(1024)
{
	addEpollFdRead(epollfd_, listenfd);
}


EpollPoller::~EpollPoller()
{
	::close(epollfd_);
}

void EpollPoller::loop()
{
	isLooping_ = true;
	while(isLooping_)
	{
		waitEpollfd();
	}
}

void EpollPoller::unloop()
{
	if(isLooping_)
		isLooping_ = false;
}


void EpollPoller::setConnectionCallback(EpollCallback cb)
{
	onConnectionCb_ = cb;
}

void EpollPoller::setMessageCallback(EpollCallback cb)
{
	onMessageCb_ = cb;
}

void EpollPoller::setCloseCallback(EpollCallback cb)
{
	onCloseCb_ = cb;
}

void EpollPoller::waitEpollfd()
{
	int nready;
	do
	{
		nready = ::epoll_wait(epollfd_, 
							  &(*eventsList_.begin()),
							  eventsList_.size(),
							  5000);
	}while(nready == -1 && errno == EINTR);

	if(nready == -1)
	{
		perror("epoll_wait error");
		exit(EXIT_FAILURE);
	}
	else if(nready == 0)
	{
		printf("epoll_wait timeout\n");	
	}
	else
	{
		//做一个扩容的操作
		if(nready == static_cast<int>(eventsList_.size()))
		{
			eventsList_.resize(eventsList_.size() * 2);
		}
		
		//遍历每一个激活的文件描述符
		for(int idx = 0; idx != nready; ++idx)
		{
			if(eventsList_[idx].data.fd == listenfd_)
			{
				if(eventsList_[idx].events & EPOLLIN)
				{
					handleConnection();
				}
			}
			else
			{
				if(eventsList_[idx].events & EPOLLIN )
				{
					handleMessage(eventsList_[idx].data.fd);
				}
			}
		}//end for
	}//end else
}

void EpollPoller::handleConnection()
{
	int peerfd = acceptConnFd(listenfd_);
	addEpollFdRead(epollfd_, peerfd);

	TcpConnectionPtr conn(new TcpConnection(peerfd));
	//...给客户端发一个欢迎信息==> 挖一个空: 等...
	//conn->send("welcome to server.\n");
	conn->setConnectionCallback(onConnectionCb_);
	conn->setMessageCallback(onMessageCb_);
	conn->setCloseCallback(onCloseCb_);

	std::pair<ConnectionMap::iterator, bool> ret;
	ret = connMap_.insert(std::make_pair(peerfd, conn));
	assert(ret.second == true);
	(void)ret;
	//connMap_[peerfd] = conn;

	conn->handleConnectionCallback();
}

void EpollPoller::handleMessage(int peerfd)
{
	bool isClosed = isConnectionClosed(peerfd);
	ConnectionMap::iterator it = connMap_.find(peerfd);
	assert(it != connMap_.end());

	if(isClosed)
	{
		it->second->handleCloseCallback();
		delEpollReadFd(epollfd_, peerfd);
		connMap_.erase(it);
	}
	else
	{
		it->second->handleMessageCallback();
	}
}

}// end of namespace wd
