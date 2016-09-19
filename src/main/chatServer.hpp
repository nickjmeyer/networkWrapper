#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "networkWrapper.hpp"
#include "chat.pb.h"
#include <queue>

class ChatServer;
class ChatConnection;
class ChatAcceptor;

class ChatServer {
public:
	void process(const chat::Letter & letter,
		const boost::shared_ptr<ChatConnection> connection);

	void addConnection(const std::string & hostPort,
		const boost::shared_ptr<ChatConnection> connection);

	void delConnection(const boost::shared_ptr<ChatConnection> connection);

private:
	// host and port to connection
	std::map<const std::string,const boost::shared_ptr<ChatConnection> > hpToConn;
};


class ChatConnection : public Connection
{
private:
	boost::shared_ptr<ChatServer> chatSrv;

	void OnAccept( const std::string & host, uint16_t port );

	void OnConnect( const std::string & host, uint16_t port );

	void OnSend( const std::vector< uint8_t > & buffer );

	void OnRecv( std::vector< uint8_t > & buffer );

	void OnTimer( const boost::posix_time::time_duration & delta );

	void OnError( const boost::system::error_code & error );

public:
	ChatConnection( boost::shared_ptr<ChatServer> chatSrv,
		boost::shared_ptr< Hive > hive );


	~ChatConnection();

	boost::shared_ptr<Connection> NewConnection();

	void SendLetter(const chat::Letter & letter);
};

class ChatAcceptor : public Acceptor
{
private:
	boost::shared_ptr<ChatServer> chatSrv;

	bool OnAccept( boost::shared_ptr< Connection > connection,
		const std::string & host, uint16_t port );

	void OnTimer( const boost::posix_time::time_duration & delta );

	void OnError( const boost::system::error_code & error );

public:
	ChatAcceptor( boost::shared_ptr<ChatServer> chatSrv,
		boost::shared_ptr< Hive > hive );

	~ChatAcceptor();
};

#endif
