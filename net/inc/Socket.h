#ifndef _SOCKET_H
#define _SOCKET_H

#include "noncopyable.h"

namespace elton
{
class InetAddress;
class Socket : NonCopyable
{
public:
	Socket(int sockfd);
	Socket();
	~Socket();

	void ready(const InetAddress & addr);

	int accept();
	void shutdownWrite();
	int fd(){	return sockfd_;	}

	static InetAddress getLocalAddr(int sockfd);
	static InetAddress getPeerAddr(int sockfd);

private:
	void bindAddress(const InetAddress & addr);
	void listen();
	void setReuseAddr(bool flag);
	void setReusePort(bool flag);
private:
	int sockfd_;
};
}// end of namespace wd

#endif
