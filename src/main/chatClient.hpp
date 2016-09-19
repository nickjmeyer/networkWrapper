#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

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

	boost::shared_ptr<Connection> NewConnection();

	void SendLetter(const chat::Letter & letter);
};

#endif
