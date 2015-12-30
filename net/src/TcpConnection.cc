#include "TcpConnection.h"
#include <string.h>
#include <stdio.h>

#include <iostream>
namespace elton
{

TcpConnection::TcpConnection(int sockfd)
	: sockfd_(sockfd),
	  sockIO_(sockfd),
	  localAddr_(elton::Socket::getLocalAddr(sockfd)),
	  peerAddr_(elton::Socket::getPeerAddr(sockfd)),
	  isShutdownWrite_(false)
{}


TcpConnection::~TcpConnection()
{
	if(!isShutdownWrite_)
	{
		isShutdownWrite_ = true;
		shutdown();
	}
}

std::string TcpConnection::receive()
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	size_t ret = sockIO_.readline(buf, sizeof(buf));
	if(ret == 0)
	{
		return std::string();
	}
	else
		return std::string(buf);
}

void TcpConnection::send(const std::string & msg)
{

	std::cout << msg <<  __func__ << std::endl;
	std::cout << msg.size() << __func__ << std::endl;
	sockIO_.writen(msg.c_str(), msg.size());
}


void TcpConnection::shutdown()
{
	sockfd_.shutdownWrite();
}

std::string TcpConnection::toString()
{
	char str[100];
	snprintf(str, sizeof(str), "%s:%d -> %s:%d",
			 localAddr_.ip().c_str(),
			 localAddr_.port(),
			 peerAddr_.ip().c_str(),
			 peerAddr_.port());
	return std::string(str);
}


void TcpConnection::setConnectionCallback(TcpConnectionCallback cb)
{
	onConnectionCb_ = cb;
}

void TcpConnection::setMessageCallback(TcpConnectionCallback cb)
{
	onMessageCb_ = cb;
}

void TcpConnection::setCloseCallback(TcpConnectionCallback cb)
{
	onCloseCb_ = cb;
}

void TcpConnection::handleConnectionCallback()
{
	if(onConnectionCb_)
		onConnectionCb_(shared_from_this());
}

void TcpConnection::handleMessageCallback()
{
	if(onMessageCb_)
		onMessageCb_(shared_from_this());
}

void TcpConnection::handleCloseCallback()
{
	if(onCloseCb_)
		onCloseCb_(shared_from_this());
}

}// end of namespace wd
