 ///
 /// @file    TcpConnection.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2015-11-05 16:59:04
 ///

#ifndef _TCPCONNCETION_H
#define _TCPCONNCETION_H

#include "noncopyable.h"
#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"

#include <string>
#include <memory>
#include <functional>

namespace elton
{

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : NonCopyable,
	public std::enable_shared_from_this<TcpConnection>
{
public:
	typedef std::function<void(const TcpConnectionPtr &)> TcpConnectionCallback;
	TcpConnection(int sockfd);
	~TcpConnection();

	std::string receive();
	void send(const std::string & msg);
	void shutdown();

	std::string toString();

	void setConnectionCallback(TcpConnectionCallback cb);
	void setMessageCallback(TcpConnectionCallback cb);
	void setCloseCallback(TcpConnectionCallback cb);

	void handleConnectionCallback();
	void handleMessageCallback();
	void handleCloseCallback();

private:
	Socket sockfd_;
	SocketIO sockIO_;
	const InetAddress localAddr_;
	const InetAddress peerAddr_;
	bool isShutdownWrite_;

	TcpConnectionCallback onConnectionCb_;
	TcpConnectionCallback onMessageCb_;
	TcpConnectionCallback onCloseCb_;

};

}//end of namespace wd

#endif
