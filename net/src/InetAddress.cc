 ///
 /// @file    InetAddress.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-04 17:14:06
 ///


#include "InetAddress.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

namespace elton
{

InetAddress::InetAddress(short port)
{
	::memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = INADDR_ANY;
}


InetAddress::InetAddress(const char * pIp, short port)
{
	::memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = inet_addr(pIp);
}

InetAddress::InetAddress(const struct sockaddr_in & addr)
	: addr_(addr)
{}

const struct sockaddr_in * InetAddress::getSockAddrPtr() const
{
	return &addr_;
}

std::string InetAddress::ip() const
{
	return std::string(inet_ntoa(addr_.sin_addr));
}

unsigned short InetAddress::port() const
{
	return ntohs(addr_.sin_port);
}


}// end of namespace elton

