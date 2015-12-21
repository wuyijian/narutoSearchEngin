 ///
 /// @file    InetAddress.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-04 17:08:29
 ///


#ifndef _INETADDRESS_H
#define _INETADDRESS_H


#include <netinet/in.h>
#include <string>

namespace elton
{

class InetAddress
{
public:
	InetAddress(short port);

	InetAddress(const char * pIp, short port);

	InetAddress(const struct sockaddr_in & addr);

	const struct sockaddr_in * getSockAddrPtr() const;
	std::string ip() const;
	unsigned short port() const;

private:
	struct sockaddr_in addr_;
};


}// end of namespace wd



#endif
