#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "networkWrapper.hpp"
#include "chat.pb.h"
#include <queue>

class ChatClient;
class ChatConnection;
class ChatAcceptor;

class ChatClient {
public:

private:
	std::queue<chat::Letter> outgoing;
	std::queue<chat::Letter> incoming;
};


class ChatConnection : public Connection
{
private:
	void OnAccept( const std::string & host, uint16_t port );

	void OnConnect( const std::string & host, uint16_t port );

	void OnSend( const std::vector< uint8_t > & buffer );

	void OnRecv( std::vector< uint8_t > & buffer );

	void OnTimer( const boost::posix_time::time_duration & delta );

	void OnError( const boost::system::error_code & error );

public:
	ChatConnection( boost::shared_ptr< Hive > hive );

	~ChatConnection();
};

class ChatAcceptor : public Acceptor
{
private:
	bool OnAccept( boost::shared_ptr< Connection > connection,
		const std::string & host, uint16_t port );

	void OnTimer( const boost::posix_time::time_duration & delta );

	void OnError( const boost::system::error_code & error );

public:
	ChatAcceptor( boost::shared_ptr< Hive > hive );

	~ChatAcceptor();
};

#endif
